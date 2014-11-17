#include "interpreter.h"

struct Table StringTable;
struct TableEntry *StringHashTable[STRING_TABLE_SIZE];
char *StrEmpty = NULL;

/* initialise the shared string system */
void TableInit()
{
    TableInitTable(&StringTable, &StringHashTable[0], STRING_TABLE_SIZE, TRUE);
    StrEmpty = TableStrRegister("");
}

/* hash function for strings */
static unsigned int TableHash(const char *Key, int Len)
{
    unsigned int Hash = Len;
    int Offset;
    int Count;
    
    for (Count = 0, Offset = 8; Count < Len; Count++, Offset+=7)
    {
        if (Offset > sizeof(unsigned int) * 8 - 7)
            Offset -= sizeof(unsigned int) * 8 - 6;
            
        Hash ^= *Key++ << Offset;
    }
    
    return Hash;
}

/* initialise a table */
void TableInitTable(struct Table *Tbl, struct TableEntry **HashTable, int Size, int OnHeap)
{
    Tbl->Size = Size;
    Tbl->OnHeap = OnHeap;
    Tbl->HashTable = HashTable;
    memset((void *)HashTable, '\0', sizeof(struct TableEntry *) * Size);
}

/* check a hash table entry for a key */
static struct TableEntry *TableSearch(struct Table *Tbl, const char *Key, int *AddAt)
{
    struct TableEntry *Entry;
    int HashValue = ((unsigned long)Key) % Tbl->Size;   /* shared strings have unique addresses so we don't need to hash them */
    
    for (Entry = Tbl->HashTable[HashValue]; Entry != NULL; Entry = Entry->Next)
    {
        if (Entry->p.v.Key == Key)
            return Entry;   /* found */
    }
    
    *AddAt = HashValue;    /* didn't find it in the chain */
    return NULL;
}

/* set an identifier to a value. returns FALSE if it already exists. 
 * Key must be a shared string from TableStrRegister() */
int TableSet(struct Table *Tbl, char *Key, struct Value *Val, const char *DeclFileName, int DeclLine, int DeclColumn)
{
    int AddAt;
    struct TableEntry *FoundEntry = TableSearch(Tbl, Key, &AddAt);
    
    if (FoundEntry == NULL)
    {   /* add it to the table */
        struct TableEntry *NewEntry = VariableAlloc(NULL, sizeof(struct TableEntry), Tbl->OnHeap);
        NewEntry->DeclFileName = DeclFileName;
        NewEntry->DeclLine = DeclLine;
        NewEntry->DeclColumn = DeclColumn;
        NewEntry->p.v.Key = Key;
        NewEntry->p.v.Val = Val;
        NewEntry->Next = Tbl->HashTable[AddAt];
        Tbl->HashTable[AddAt] = NewEntry;
        return TRUE;
    }

    return FALSE;
}

/* find a value in a table. returns FALSE if not found. 
 * Key must be a shared string from TableStrRegister() */
int TableGet(struct Table *Tbl, const char *Key, struct Value **Val, const char **DeclFileName, int *DeclLine, int *DeclColumn)
{
    int AddAt;
    struct TableEntry *FoundEntry = TableSearch(Tbl, Key, &AddAt);
    if (FoundEntry == NULL)
        return FALSE;
    
    *Val = FoundEntry->p.v.Val;
    
    if (DeclFileName != NULL)
    {
        *DeclFileName = FoundEntry->DeclFileName;
        *DeclLine = FoundEntry->DeclLine;
        *DeclColumn = FoundEntry->DeclColumn;
    }
    
    return TRUE;
}

/* remove an entry from the table */
struct Value *TableDelete(struct Table *Tbl, const char *Key)
{
    struct TableEntry **EntryPtr;
    int HashValue = ((unsigned long)Key) % Tbl->Size;   /* shared strings have unique addresses so we don't need to hash them */
    
    for (EntryPtr = &Tbl->HashTable[HashValue]; *EntryPtr != NULL; EntryPtr = &(*EntryPtr)->Next)
    {
        if ((*EntryPtr)->p.v.Key == Key)
        {
            struct TableEntry *DeleteEntry = *EntryPtr;
            struct Value *Val = DeleteEntry->p.v.Val;
            *EntryPtr = DeleteEntry->Next;
            HeapFreeMem(DeleteEntry);

            return Val;
        }
    }

    return NULL;
}

/* check a hash table entry for an identifier */
static struct TableEntry *TableSearchIdentifier(struct Table *Tbl, const char *Key, int Len, int *AddAt)
{
    struct TableEntry *Entry;
    int HashValue = TableHash(Key, Len) % Tbl->Size;
    
    for (Entry = Tbl->HashTable[HashValue]; Entry != NULL; Entry = Entry->Next)
    {
        if (strncmp(&Entry->p.Key[0], (char *)Key, Len) == 0 && Entry->p.Key[Len] == '\0')
            return Entry;   /* found */
    }
    
    *AddAt = HashValue;    /* didn't find it in the chain */
    return NULL;
}

/* set an identifier and return the identifier. share if possible */
char *TableSetIdentifier(struct Table *Tbl, const char *Ident, int IdentLen)
{
    int AddAt;
    struct TableEntry *FoundEntry = TableSearchIdentifier(Tbl, Ident, IdentLen, &AddAt);
    
    if (FoundEntry != NULL)
        return &FoundEntry->p.Key[0];
    else
    {   /* add it to the table - we economise by not allocating the whole structure here */
        struct TableEntry *NewEntry = HeapAllocMem(sizeof(struct TableEntry) - sizeof(union TableEntryPayload) + IdentLen + 1);
        if (NewEntry == NULL)
            ProgramFail(NULL, "out of memory");
            
        strncpy((char *)&NewEntry->p.Key[0], (char *)Ident, IdentLen);
        NewEntry->p.Key[IdentLen] = '\0';
        NewEntry->Next = Tbl->HashTable[AddAt];
        Tbl->HashTable[AddAt] = NewEntry;
        return &NewEntry->p.Key[0];
    }
}

/* register a string in the shared string store */
char *TableStrRegister2(const char *Str, int Len)
{
    return TableSetIdentifier(&StringTable, Str, Len);
}

char *TableStrRegister(const char *Str)
{
    return TableStrRegister2(Str, strlen((char *)Str));
}

/* free all the strings */
void TableStrFree()
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < StringTable.Size; Count++)
    {
        for (Entry = StringTable.HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            HeapFreeMem(Entry);
        }
    }
}
