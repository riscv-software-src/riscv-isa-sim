/* stack grows up from the bottom and heap grows down from the top of heap space */
#include "interpreter.h"

#define FREELIST_BUCKETS 8                          /* freelists for 4, 8, 12 ... 32 byte allocs */
#define SPLIT_MEM_THRESHOLD 16                      /* don't split memory which is close in size */

#ifdef USE_MALLOC_STACK
static unsigned char *HeapMemory = NULL;            /* stack memory since our heap is malloc()ed */
static void *HeapBottom = NULL;                     /* the bottom of the (downward-growing) heap */
static void *StackFrame = NULL;                     /* the current stack frame */
void *HeapStackTop = NULL;                          /* the top of the stack */
#else
# ifdef SURVEYOR_HOST
static unsigned char *HeapMemory = (unsigned char *)C_HEAPSTART;      /* all memory - stack and heap */
static void *HeapBottom = (void *)C_HEAPSTART + HEAP_SIZE;  /* the bottom of the (downward-growing) heap */
static void *StackFrame = (void *)C_HEAPSTART;              /* the current stack frame */
void *HeapStackTop = (void *)C_HEAPSTART;                   /* the top of the stack */
void *HeapMemStart = (void *)C_HEAPSTART;
# else
static unsigned char HeapMemory[HEAP_SIZE];         /* all memory - stack and heap */
static void *HeapBottom = &HeapMemory[HEAP_SIZE];   /* the bottom of the (downward-growing) heap */
static void *StackFrame = &HeapMemory[0];           /* the current stack frame */
void *HeapStackTop = &HeapMemory[0];                /* the top of the stack */
# endif
#endif

static struct AllocNode *FreeListBucket[FREELIST_BUCKETS];      /* we keep a pool of freelist buckets to reduce fragmentation */
static struct AllocNode *FreeListBig;                           /* free memory which doesn't fit in a bucket */

#ifdef DEBUG_HEAP
void ShowBigList()
{
    struct AllocNode *LPos;
    
    printf("Heap: bottom=0x%lx 0x%lx-0x%lx, big freelist=", (long)HeapBottom, (long)&HeapMemory[0], (long)&HeapMemory[HEAP_SIZE]);
    for (LPos = FreeListBig; LPos != NULL; LPos = LPos->NextFree)
        printf("0x%lx:%d ", (long)LPos, LPos->Size);
    
    printf("\n");
}
#endif

/* initialise the stack and heap storage */
void HeapInit(int StackOrHeapSize)
{
    int Count;
    int AlignOffset = 0;
    
#ifdef USE_MALLOC_STACK
    HeapMemory = malloc(StackOrHeapSize);
#endif

    while (((unsigned long)&HeapMemory[AlignOffset] & (sizeof(ALIGN_TYPE)-1)) != 0)
        AlignOffset++;
        
    StackFrame = &HeapMemory[AlignOffset];
    HeapStackTop = &HeapMemory[AlignOffset];
    *(void **)StackFrame = NULL;
    HeapBottom = &HeapMemory[StackOrHeapSize-sizeof(ALIGN_TYPE)+AlignOffset];
    FreeListBig = NULL;
    for (Count = 0; Count < FREELIST_BUCKETS; Count++)
        FreeListBucket[Count] = NULL;
}

void HeapCleanup()
{
#ifdef USE_MALLOC_STACK
    free(HeapMemory);
#endif
}

/* allocate some space on the stack, in the current stack frame
 * clears memory. can return NULL if out of stack space */
void *HeapAllocStack(int Size)
{
    char *NewMem = HeapStackTop;
    char *NewTop = (char *)HeapStackTop + MEM_ALIGN(Size);
#ifdef DEBUG_HEAP
    printf("HeapAllocStack(%ld) at 0x%lx\n", (unsigned long)MEM_ALIGN(Size), (unsigned long)HeapStackTop);
#endif
    if (NewTop > (char *)HeapBottom)
        return NULL;
        
    HeapStackTop = (void *)NewTop;
    memset((void *)NewMem, '\0', Size);
    return NewMem;
}

/* allocate some space on the stack, in the current stack frame */
void HeapUnpopStack(int Size)
{
#ifdef DEBUG_HEAP
    printf("HeapUnpopStack(%ld) at 0x%lx\n", (unsigned long)MEM_ALIGN(Size), (unsigned long)HeapStackTop);
#endif
    HeapStackTop = (void *)((char *)HeapStackTop + MEM_ALIGN(Size));
}

/* free some space at the top of the stack */
int HeapPopStack(void *Addr, int Size)
{
    int ToLose = MEM_ALIGN(Size);
    if (ToLose > ((char *)HeapStackTop - (char *)&HeapMemory[0]))
        return FALSE;
    
#ifdef DEBUG_HEAP
    printf("HeapPopStack(0x%lx, %ld) back to 0x%lx\n", (unsigned long)Addr, (unsigned long)MEM_ALIGN(Size), (unsigned long)HeapStackTop - ToLose);
#endif
    HeapStackTop = (void *)((char *)HeapStackTop - ToLose);
    assert(Addr == NULL || HeapStackTop == Addr);
    
    return TRUE;
}

/* push a new stack frame on to the stack */
void HeapPushStackFrame()
{
#ifdef DEBUG_HEAP
    printf("Adding stack frame at 0x%lx\n", (unsigned long)HeapStackTop);
#endif
    *(void **)HeapStackTop = StackFrame;
    StackFrame = HeapStackTop;
    HeapStackTop = (void *)((char *)HeapStackTop + MEM_ALIGN(sizeof(ALIGN_TYPE)));
}

/* pop the current stack frame, freeing all memory in the frame. can return NULL */
int HeapPopStackFrame()
{
    if (*(void **)StackFrame != NULL)
    {
        HeapStackTop = StackFrame;
        StackFrame = *(void **)StackFrame;
#ifdef DEBUG_HEAP
        printf("Popping stack frame back to 0x%lx\n", (unsigned long)HeapStackTop);
#endif
        return TRUE;
    }
    else
        return FALSE;
}

/* allocate some dynamically allocated memory. memory is cleared. can return NULL if out of memory */
void *HeapAllocMem(int Size)
{
#ifdef USE_MALLOC_HEAP
    return calloc(Size, 1);
#else
    struct AllocNode *NewMem = NULL;
    struct AllocNode **FreeNode;
    int AllocSize = MEM_ALIGN(Size) + MEM_ALIGN(sizeof(NewMem->Size));
    int Bucket;
    void *ReturnMem;
    
    if (Size == 0)
        return NULL;
    
    assert(Size > 0);
    
    /* make sure we have enough space for an AllocNode */
    if (AllocSize < sizeof(struct AllocNode))
        AllocSize = sizeof(struct AllocNode);
    
    Bucket = AllocSize >> 2;
    if (Bucket < FREELIST_BUCKETS && FreeListBucket[Bucket] != NULL)
    { 
        /* try to allocate from a freelist bucket first */
#ifdef DEBUG_HEAP
        printf("allocating %d(%d) from bucket", Size, AllocSize);
#endif
        NewMem = FreeListBucket[Bucket];
        assert((unsigned long)NewMem >= (unsigned long)&HeapMemory[0] && (unsigned char *)NewMem - &HeapMemory[0] < HEAP_SIZE);
        FreeListBucket[Bucket] = *(struct AllocNode **)NewMem;
        assert(FreeListBucket[Bucket] == NULL || ((unsigned long)FreeListBucket[Bucket] >= (unsigned long)&HeapMemory[0] && (unsigned char *)FreeListBucket[Bucket] - &HeapMemory[0] < HEAP_SIZE));
        NewMem->Size = AllocSize;
    }
    else if (FreeListBig != NULL)
    { 
        /* grab the first item from the "big" freelist we can fit in */
        for (FreeNode = &FreeListBig; *FreeNode != NULL && (*FreeNode)->Size < AllocSize; FreeNode = &(*FreeNode)->NextFree)
        {}
        
        if (*FreeNode != NULL)
        {
            assert((unsigned long)*FreeNode >= (unsigned long)&HeapMemory[0] && (unsigned char *)*FreeNode - &HeapMemory[0] < HEAP_SIZE);
            assert((*FreeNode)->Size < HEAP_SIZE && (*FreeNode)->Size > 0);
            if ((*FreeNode)->Size < AllocSize + SPLIT_MEM_THRESHOLD)
            { 
                /* close in size - reduce fragmentation by not splitting */
#ifdef DEBUG_HEAP
               printf("allocating %d(%d) from freelist, no split (%d)", Size, AllocSize, (*FreeNode)->Size);
#endif
                NewMem = *FreeNode;
                assert((unsigned long)NewMem >= (unsigned long)&HeapMemory[0] && (unsigned char *)NewMem - &HeapMemory[0] < HEAP_SIZE);
                *FreeNode = NewMem->NextFree;
            }
            else
            { 
                /* split this big memory chunk */
#ifdef DEBUG_HEAP
                printf("allocating %d(%d) from freelist, split chunk (%d)", Size, AllocSize, (*FreeNode)->Size);
#endif
                NewMem = (void *)((char *)*FreeNode + (*FreeNode)->Size - AllocSize);
                assert((unsigned long)NewMem >= (unsigned long)&HeapMemory[0] && (unsigned char *)NewMem - &HeapMemory[0] < HEAP_SIZE);
                (*FreeNode)->Size -= AllocSize;
                NewMem->Size = AllocSize;
            }
        }
    }
    
    if (NewMem == NULL)
    { 
        /* couldn't allocate from a freelist - try to increase the size of the heap area */
#ifdef DEBUG_HEAP
        printf("allocating %d(%d) at bottom of heap (0x%lx-0x%lx)", Size, AllocSize, (long)((char *)HeapBottom - AllocSize), (long)HeapBottom);
#endif
        if ((char *)HeapBottom - AllocSize < (char *)HeapStackTop)
            return NULL;
        
        HeapBottom = (void *)((char *)HeapBottom - AllocSize);
        NewMem = HeapBottom;
        NewMem->Size = AllocSize;
    }
    
    ReturnMem = (void *)((char *)NewMem + MEM_ALIGN(sizeof(NewMem->Size)));
    memset(ReturnMem, '\0', AllocSize - MEM_ALIGN(sizeof(NewMem->Size)));
#ifdef DEBUG_HEAP
    printf(" = %lx\n", (unsigned long)ReturnMem);
#endif
    return ReturnMem;
#endif
}

/* free some dynamically allocated memory */
void HeapFreeMem(void *Mem)
{
#ifdef USE_MALLOC_HEAP
    free(Mem);
#else
    struct AllocNode *MemNode = (struct AllocNode *)((char *)Mem - MEM_ALIGN(sizeof(MemNode->Size)));
    int Bucket = MemNode->Size >> 2;
    
#ifdef DEBUG_HEAP
    printf("HeapFreeMem(0x%lx)\n", (unsigned long)Mem);
#endif
    assert((unsigned long)Mem >= (unsigned long)&HeapMemory[0] && (unsigned char *)Mem - &HeapMemory[0] < HEAP_SIZE);
    assert(MemNode->Size < HEAP_SIZE && MemNode->Size > 0);
    if (Mem == NULL)
        return;
    
    if ((void *)MemNode == HeapBottom)
    { 
        /* pop it off the bottom of the heap, reducing the heap size */
#ifdef DEBUG_HEAP
        printf("freeing %d from bottom of heap\n", MemNode->Size);
#endif
        HeapBottom = (void *)((char *)HeapBottom + MemNode->Size);
#ifdef DEBUG_HEAP
        ShowBigList();
#endif
    }
    else if (Bucket < FREELIST_BUCKETS)
    { 
        /* we can fit it in a bucket */
#ifdef DEBUG_HEAP
        printf("freeing %d to bucket\n", MemNode->Size);
#endif
        assert(FreeListBucket[Bucket] == NULL || ((unsigned long)FreeListBucket[Bucket] >= (unsigned long)&HeapMemory[0] && (unsigned char *)FreeListBucket[Bucket] - &HeapMemory[0] < HEAP_SIZE));
        *(struct AllocNode **)MemNode = FreeListBucket[Bucket];
        FreeListBucket[Bucket] = (struct AllocNode *)MemNode;
    }
    else
    { 
        /* put it in the big memory freelist */
#ifdef DEBUG_HEAP
        printf("freeing %lx:%d to freelist\n", (unsigned long)Mem, MemNode->Size);
#endif
        assert(FreeListBig == NULL || ((unsigned long)FreeListBig >= (unsigned long)&HeapMemory[0] && (unsigned char *)FreeListBig - &HeapMemory[0] < HEAP_SIZE));
        MemNode->NextFree = FreeListBig;
        FreeListBig = MemNode;
#ifdef DEBUG_HEAP
        ShowBigList();
#endif
    }
#endif
}

