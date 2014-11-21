#include "interpreter.h"

/* some basic types */
struct ValueType UberType;
struct ValueType IntType;
struct ValueType ShortType;
struct ValueType CharType;
struct ValueType LongType;
struct ValueType UnsignedIntType;
struct ValueType UnsignedShortType;
struct ValueType UnsignedLongType;
#ifndef NO_FP
struct ValueType FPType;
#endif
struct ValueType VoidType;
struct ValueType TypeType;
struct ValueType FunctionType;
struct ValueType MacroType;
struct ValueType EnumType;
struct ValueType GotoLabelType;
struct ValueType *CharPtrType;
struct ValueType *CharPtrPtrType;
struct ValueType *CharArrayType;
struct ValueType *VoidPtrType;

static int PointerAlignBytes;
static int IntAlignBytes;


/* add a new type to the set of types we know about */
struct ValueType *TypeAdd(struct ParseState *Parser, struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int Sizeof, int AlignBytes)
{
    struct ValueType *NewType = VariableAlloc(Parser, sizeof(struct ValueType), TRUE);
    NewType->Base = Base;
    NewType->ArraySize = ArraySize;
    NewType->Sizeof = Sizeof;
    NewType->AlignBytes = AlignBytes;
    NewType->Identifier = Identifier;
    NewType->Members = NULL;
    NewType->FromType = ParentType;
    NewType->DerivedTypeList = NULL;
    NewType->OnHeap = TRUE;
    NewType->Next = ParentType->DerivedTypeList;
    ParentType->DerivedTypeList = NewType;
    
    return NewType;
}

/* given a parent type, get a matching derived type and make one if necessary.
 * Identifier should be registered with the shared string table. */
struct ValueType *TypeGetMatching(struct ParseState *Parser, struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int AllowDuplicates)
{
    int Sizeof;
    int AlignBytes;
    struct ValueType *ThisType = ParentType->DerivedTypeList;
    while (ThisType != NULL && (ThisType->Base != Base || ThisType->ArraySize != ArraySize || ThisType->Identifier != Identifier))
        ThisType = ThisType->Next;
    
    if (ThisType != NULL)
    {
        if (AllowDuplicates)
            return ThisType;
        else
            ProgramFail(Parser, "data type '%s' is already defined", Identifier);
    }
        
    switch (Base)
    {
        case TypePointer:   Sizeof = sizeof(void *); AlignBytes = PointerAlignBytes; break;
        case TypeArray:     Sizeof = ArraySize * ParentType->Sizeof; AlignBytes = ParentType->AlignBytes; break;
        case TypeEnum:      Sizeof = sizeof(int); AlignBytes = IntAlignBytes; break;
        default:            Sizeof = 0; AlignBytes = 0; break;      /* structs and unions will get bigger when we add members to them */
    }

    return TypeAdd(Parser, ParentType, Base, ArraySize, Identifier, Sizeof, AlignBytes);
}

/* stack space used by a value */
int TypeStackSizeValue(struct Value *Val)
{
    if (Val != NULL && Val->ValOnStack)
        return TypeSizeValue(Val, FALSE);
    else
        return 0;
}

/* memory used by a value */
int TypeSizeValue(struct Value *Val, int Compact)
{
    if (IS_INTEGER_NUMERIC(Val) && !Compact)
        return sizeof(ALIGN_TYPE);     /* allow some extra room for type extension */
    else if (Val->Typ->Base != TypeArray)
        return Val->Typ->Sizeof;
    else
        return Val->Typ->FromType->Sizeof * Val->Typ->ArraySize;
}

/* memory used by a variable given its type and array size */
int TypeSize(struct ValueType *Typ, int ArraySize, int Compact)
{
    if (IS_INTEGER_NUMERIC_TYPE(Typ) && !Compact)
        return sizeof(ALIGN_TYPE);     /* allow some extra room for type extension */
    else if (Typ->Base != TypeArray)
        return Typ->Sizeof;
    else
        return Typ->FromType->Sizeof * ArraySize;
}

/* add a base type */
void TypeAddBaseType(struct ValueType *TypeNode, enum BaseType Base, int Sizeof, int AlignBytes)
{
    TypeNode->Base = Base;
    TypeNode->ArraySize = 0;
    TypeNode->Sizeof = Sizeof;
    TypeNode->AlignBytes = AlignBytes;
    TypeNode->Identifier = StrEmpty;
    TypeNode->Members = NULL;
    TypeNode->FromType = NULL;
    TypeNode->DerivedTypeList = NULL;
    TypeNode->OnHeap = FALSE;
    TypeNode->Next = UberType.DerivedTypeList;
    UberType.DerivedTypeList = TypeNode;
}

/* initialise the type system */
void TypeInit()
{
    struct IntAlign { char x; int y; } ia;
    struct ShortAlign { char x; short y; } sa;
    struct CharAlign { char x; char y; } ca;
    struct LongAlign { char x; long y; } la;
#ifndef NO_FP
    struct DoubleAlign { char x; double y; } da;
#endif
    struct PointerAlign { char x; void *y; } pa;
    
    IntAlignBytes = (char *)&ia.y - &ia.x;
    PointerAlignBytes = (char *)&pa.y - &pa.x;
    
    UberType.DerivedTypeList = NULL;
    TypeAddBaseType(&IntType, TypeInt, sizeof(int), IntAlignBytes);
    TypeAddBaseType(&ShortType, TypeShort, sizeof(short), (char *)&sa.y - &sa.x);
    TypeAddBaseType(&CharType, TypeChar, sizeof(unsigned char), (char *)&ca.y - &ca.x);
    TypeAddBaseType(&LongType, TypeLong, sizeof(long), (char *)&la.y - &la.x);
    TypeAddBaseType(&UnsignedIntType, TypeUnsignedInt, sizeof(unsigned int), IntAlignBytes);
    TypeAddBaseType(&UnsignedShortType, TypeUnsignedShort, sizeof(unsigned short), (char *)&sa.y - &sa.x);
    TypeAddBaseType(&UnsignedLongType, TypeUnsignedLong, sizeof(unsigned long), (char *)&la.y - &la.x);
    TypeAddBaseType(&VoidType, TypeVoid, 0, 1);
    TypeAddBaseType(&FunctionType, TypeFunction, sizeof(int), IntAlignBytes);
    TypeAddBaseType(&MacroType, TypeMacro, sizeof(int), IntAlignBytes);
    TypeAddBaseType(&GotoLabelType, TypeGotoLabel, 0, 1);
#ifndef NO_FP
    TypeAddBaseType(&FPType, TypeFP, sizeof(double), (char *)&da.y - &da.x);
    TypeAddBaseType(&TypeType, Type_Type, sizeof(double), (char *)&da.y - &da.x);  /* must be large enough to cast to a double */
#else
    TypeAddBaseType(&TypeType, Type_Type, sizeof(struct ValueType *), PointerAlignBytes);
#endif
    CharArrayType = TypeAdd(NULL, &CharType, TypeArray, 0, StrEmpty, sizeof(char), (char *)&ca.y - &ca.x);
    CharPtrType = TypeAdd(NULL, &CharType, TypePointer, 0, StrEmpty, sizeof(void *), PointerAlignBytes);
    CharPtrPtrType = TypeAdd(NULL, CharPtrType, TypePointer, 0, StrEmpty, sizeof(void *), PointerAlignBytes);
    VoidPtrType = TypeAdd(NULL, &VoidType, TypePointer, 0, StrEmpty, sizeof(void *), PointerAlignBytes);
}

/* deallocate heap-allocated types */
void TypeCleanupNode(struct ValueType *Typ)
{
    struct ValueType *SubType;
    struct ValueType *NextSubType;
    
    /* clean up and free all the sub-nodes */
    for (SubType = Typ->DerivedTypeList; SubType != NULL; SubType = NextSubType)
    {
        NextSubType = SubType->Next;
        TypeCleanupNode(SubType);
        if (SubType->OnHeap)
        {
            /* if it's a struct or union deallocate all the member values */
            if (SubType->Members != NULL)
            {
                VariableTableCleanup(SubType->Members);
                HeapFreeMem(SubType->Members);
            }

            /* free this node */
            HeapFreeMem(SubType);
        }
    }
}

void TypeCleanup()
{
    TypeCleanupNode(&UberType);
}

/* parse a struct or union declaration */
void TypeParseStruct(struct ParseState *Parser, struct ValueType **Typ, int IsStruct)
{
    struct Value *LexValue;
    struct ValueType *MemberType;
    char *MemberIdentifier;
    char *StructIdentifier;
    struct Value *MemberValue;
    enum LexToken Token;
    int AlignBoundary;
    
    Token = LexGetToken(Parser, &LexValue, FALSE);
    if (Token == TokenIdentifier)
    {
        LexGetToken(Parser, &LexValue, TRUE);
        StructIdentifier = LexValue->Val->Identifier;
        Token = LexGetToken(Parser, NULL, FALSE);
    }
    else
    {
        static char TempNameBuf[7] = "^s0000";
        StructIdentifier = PlatformMakeTempName(TempNameBuf);
    }

    *Typ = TypeGetMatching(Parser, &UberType, IsStruct ? TypeStruct : TypeUnion, 0, StructIdentifier, Token != TokenLeftBrace);

    Token = LexGetToken(Parser, NULL, FALSE);
    if (Token != TokenLeftBrace)
    { 
        /* use the already defined structure */
        if ((*Typ)->Members == NULL)
            ProgramFail(Parser, "structure '%s' isn't defined", LexValue->Val->Identifier);
            
        return;
    }
    
    if (TopStackFrame != NULL)
        ProgramFail(Parser, "struct/union definitions can only be globals");
        
    LexGetToken(Parser, NULL, TRUE);    
    (*Typ)->Members = VariableAlloc(Parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE);
    (*Typ)->Members->HashTable = (struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table));
    TableInitTable((*Typ)->Members, (struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
    
    do {
        TypeParse(Parser, &MemberType, &MemberIdentifier, NULL);
        if (MemberType == NULL || MemberIdentifier == NULL)
            ProgramFail(Parser, "invalid type in struct");
        
        MemberValue = VariableAllocValueAndData(Parser, sizeof(int), FALSE, NULL, TRUE);
        MemberValue->Typ = MemberType;
        if (IsStruct)
        { 
            /* allocate this member's location in the struct */
            AlignBoundary = MemberValue->Typ->AlignBytes;
            if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0)
                (*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
                
            MemberValue->Val->Integer = (*Typ)->Sizeof;
            (*Typ)->Sizeof += TypeSizeValue(MemberValue, TRUE);
        }
        else
        { 
            /* union members always start at 0, make sure it's big enough to hold the largest member */
            MemberValue->Val->Integer = 0;
            if (MemberValue->Typ->Sizeof > (*Typ)->Sizeof)
                (*Typ)->Sizeof = TypeSizeValue(MemberValue, TRUE);
        }

        /* make sure to align to the size of the largest member's alignment */
        if ((*Typ)->AlignBytes < MemberValue->Typ->AlignBytes)
            (*Typ)->AlignBytes = MemberValue->Typ->AlignBytes;
        
        /* define it */
        if (!TableSet((*Typ)->Members, MemberIdentifier, MemberValue, Parser->FileName, Parser->Line, Parser->CharacterPos))
            ProgramFail(Parser, "member '%s' already defined", &MemberIdentifier);
            
        if (LexGetToken(Parser, NULL, TRUE) != TokenSemicolon)
            ProgramFail(Parser, "semicolon expected");
                    
    } while (LexGetToken(Parser, NULL, FALSE) != TokenRightBrace);
    
    /* now align the structure to the size of its largest member's alignment */
    AlignBoundary = (*Typ)->AlignBytes;
    if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0)
        (*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
    
    LexGetToken(Parser, NULL, TRUE);
}

/* create a system struct which has no user-visible members */
struct ValueType *TypeCreateOpaqueStruct(struct ParseState *Parser, const char *StructName, int Size)
{
    struct ValueType *Typ = TypeGetMatching(Parser, &UberType, TypeStruct, 0, StructName, FALSE);
    
    /* create the (empty) table */
    Typ->Members = VariableAlloc(Parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE);
    Typ->Members->HashTable = (struct TableEntry **)((char *)Typ->Members + sizeof(struct Table));
    TableInitTable(Typ->Members, (struct TableEntry **)((char *)Typ->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
    Typ->Sizeof = Size;
    
    return Typ;
}

/* parse an enum declaration */
void TypeParseEnum(struct ParseState *Parser, struct ValueType **Typ)
{
    struct Value *LexValue;
    struct Value InitValue;
    enum LexToken Token;
    struct ValueType *EnumType;
    int EnumValue = 0;
    char *EnumIdentifier;
    
    Token = LexGetToken(Parser, &LexValue, FALSE);
    if (Token == TokenIdentifier)
    {
        LexGetToken(Parser, &LexValue, TRUE);
        EnumIdentifier = LexValue->Val->Identifier;
        Token = LexGetToken(Parser, NULL, FALSE);
    }
    else
    {
        static char TempNameBuf[7] = "^e0000";
        EnumIdentifier = PlatformMakeTempName(TempNameBuf);
    }

    EnumType = TypeGetMatching(Parser, &UberType, TypeEnum, 0, EnumIdentifier, Token != TokenLeftBrace);
    *Typ = &IntType;
    if (Token != TokenLeftBrace)
    { 
        /* use the already defined enum */
        if ((*Typ)->Members == NULL)
            ProgramFail(Parser, "enum '%s' isn't defined", EnumIdentifier);
            
        return;
    }
    
    if (TopStackFrame != NULL)
        ProgramFail(Parser, "enum definitions can only be globals");
        
    LexGetToken(Parser, NULL, TRUE);    
    (*Typ)->Members = &GlobalTable;
    memset((void *)&InitValue, '\0', sizeof(struct Value));
    InitValue.Typ = &IntType;
    InitValue.Val = (union AnyValue *)&EnumValue;
    do {
        if (LexGetToken(Parser, &LexValue, TRUE) != TokenIdentifier)
            ProgramFail(Parser, "identifier expected");
        
        EnumIdentifier = LexValue->Val->Identifier;
        if (LexGetToken(Parser, NULL, FALSE) == TokenAssign)
        {
            LexGetToken(Parser, NULL, TRUE);
            EnumValue = ExpressionParseInt(Parser);
        }
        
        VariableDefine(Parser, EnumIdentifier, &InitValue, NULL, FALSE);
            
        Token = LexGetToken(Parser, NULL, TRUE);
        if (Token != TokenComma && Token != TokenRightBrace)
            ProgramFail(Parser, "comma expected");
        
        EnumValue++;
                    
    } while (Token == TokenComma);
}

/* parse a type - just the basic type */
int TypeParseFront(struct ParseState *Parser, struct ValueType **Typ, int *IsStatic)
{
    struct ParseState Before;
    struct Value *LexerValue;
    enum LexToken Token;
    int Unsigned = FALSE;
    struct Value *VarValue;
    int StaticQualifier = FALSE;
    *Typ = NULL;

    /* ignore leading type qualifiers */
    ParserCopy(&Before, Parser);
    Token = LexGetToken(Parser, &LexerValue, TRUE);
    while (Token == TokenStaticType || Token == TokenAutoType || Token == TokenRegisterType || Token == TokenExternType)
    {
        if (Token == TokenStaticType)
            StaticQualifier = TRUE;
            
        Token = LexGetToken(Parser, &LexerValue, TRUE);
    }
    
    if (IsStatic != NULL)
        *IsStatic = StaticQualifier;
        
    /* handle signed/unsigned with no trailing type */
    if (Token == TokenSignedType || Token == TokenUnsignedType)
    {
        enum LexToken FollowToken = LexGetToken(Parser, &LexerValue, FALSE);
        Unsigned = (Token == TokenUnsignedType);
        
        if (FollowToken != TokenIntType && FollowToken != TokenLongType && FollowToken != TokenShortType && FollowToken != TokenCharType)
        {
            if (Token == TokenUnsignedType)
                *Typ = &UnsignedIntType;
            else
                *Typ = &IntType;
            
            return TRUE;
        }
        
        Token = LexGetToken(Parser, &LexerValue, TRUE);
    }
    
    switch (Token)
    {
        case TokenIntType: *Typ = Unsigned ? &UnsignedIntType : &IntType; break;
        case TokenShortType: *Typ = Unsigned ? &UnsignedShortType : &ShortType; break;
        case TokenCharType: *Typ = &CharType; break;
        case TokenLongType: *Typ = Unsigned ? &UnsignedLongType : &LongType; break;
#ifndef NO_FP
        case TokenFloatType: case TokenDoubleType: *Typ = &FPType; break;
#endif
        case TokenVoidType: *Typ = &VoidType; break;
        
        case TokenStructType: case TokenUnionType: 
            if (*Typ != NULL)
                ProgramFail(Parser, "bad type declaration");
                
            TypeParseStruct(Parser, Typ, Token == TokenStructType);
            break;

        case TokenEnumType:
            if (*Typ != NULL)
                ProgramFail(Parser, "bad type declaration");
                
            TypeParseEnum(Parser, Typ);
            break;
        
        case TokenIdentifier:
            /* we already know it's a typedef-defined type because we got here */
            VariableGet(Parser, LexerValue->Val->Identifier, &VarValue);
            *Typ = VarValue->Val->Typ;
            break;

        default: ParserCopy(Parser, &Before); return FALSE;
    }
    
    return TRUE;
}

/* parse a type - the part at the end after the identifier. eg. array specifications etc. */
struct ValueType *TypeParseBack(struct ParseState *Parser, struct ValueType *FromType)
{
    enum LexToken Token;
    struct ParseState Before;

    ParserCopy(&Before, Parser);
    Token = LexGetToken(Parser, NULL, TRUE);
    if (Token == TokenLeftSquareBracket)
    {
        /* add another array bound */
        enum RunMode OldMode = Parser->Mode;
        int ArraySize;
        Parser->Mode = RunModeRun;
        ArraySize = ExpressionParseInt(Parser);
        Parser->Mode = OldMode;
        
        if (LexGetToken(Parser, NULL, TRUE) != TokenRightSquareBracket)
            ProgramFail(Parser, "']' expected");
        
        return TypeGetMatching(Parser, TypeParseBack(Parser, FromType), TypeArray, ArraySize, StrEmpty, TRUE);
    }
    else
    {
        /* the type specification has finished */
        ParserCopy(Parser, &Before);
        return FromType;
    }
}

/* parse a type - the part which is repeated with each identifier in a declaration list */
void TypeParseIdentPart(struct ParseState *Parser, struct ValueType *BasicTyp, struct ValueType **Typ, char **Identifier)
{
    struct ParseState Before;
    enum LexToken Token;
    struct Value *LexValue;
    int Done = FALSE;
    *Typ = BasicTyp;
    *Identifier = StrEmpty;
    
    while (!Done)
    {
        ParserCopy(&Before, Parser);
        Token = LexGetToken(Parser, &LexValue, TRUE);
        switch (Token)
        {
            case TokenOpenBracket:
                if (*Typ != NULL)
                    ProgramFail(Parser, "bad type declaration");
                
                TypeParse(Parser, Typ, Identifier, NULL);
                if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
                    ProgramFail(Parser, "')' expected");
                break;
                
            case TokenAsterisk:
                if (*Typ == NULL)
                    ProgramFail(Parser, "bad type declaration");

                *Typ = TypeGetMatching(Parser, *Typ, TypePointer, 0, StrEmpty, TRUE);
                break;
            
            case TokenIdentifier:
                if (*Typ == NULL || *Identifier != StrEmpty)
                    ProgramFail(Parser, "bad type declaration");
                
                *Identifier = LexValue->Val->Identifier;
                Done = TRUE;
                break;
                
            default: ParserCopy(Parser, &Before); Done = TRUE; break;
        }
    }
    
    if (*Typ == NULL)
        ProgramFail(Parser, "bad type declaration");

    if (*Identifier != StrEmpty)
    { 
        /* parse stuff after the identifier */
        *Typ = TypeParseBack(Parser, *Typ);
    }
}

/* parse a type - a complete declaration including identifier */
void TypeParse(struct ParseState *Parser, struct ValueType **Typ, char **Identifier, int *IsStatic)
{
    struct ValueType *BasicType;
    
    TypeParseFront(Parser, &BasicType, IsStatic);
    TypeParseIdentPart(Parser, BasicType, Typ, Identifier);
}

