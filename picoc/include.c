#include "picoc.h"
#include "interpreter.h"

#ifndef NO_HASH_INCLUDE

/* a list of libraries we can include */
struct IncludeLibrary
{
    char *IncludeName;
    void (*SetupFunction)(void);
    struct LibraryFunction *FuncList;
    const char *SetupCSource;
    struct IncludeLibrary *NextLib;
};

struct IncludeLibrary *IncludeLibList = NULL;


/* initialise the built-in include libraries */
void IncludeInit()
{
#ifndef BUILTIN_MINI_STDLIB
    IncludeRegister("ctype.h", NULL, &StdCtypeFunctions[0], NULL);
    IncludeRegister("errno.h", &StdErrnoSetupFunc, NULL, NULL);
#ifndef NO_FP
    IncludeRegister("math.h", &MathSetupFunc, &MathFunctions[0], NULL);
#endif
    IncludeRegister("stdbool.h", &StdboolSetupFunc, NULL, StdboolDefs);
    IncludeRegister("stdio.h", &StdioSetupFunc, &StdioFunctions[0], StdioDefs);
    IncludeRegister("stdlib.h", &StdlibSetupFunc, &StdlibFunctions[0], NULL);
    IncludeRegister("string.h", &StringSetupFunc, &StringFunctions[0], NULL);
    IncludeRegister("time.h", &StdTimeSetupFunc, &StdTimeFunctions[0], StdTimeDefs);
    IncludeRegister("unistd.h", &UnistdSetupFunc, &UnistdFunctions[0], UnistdDefs);
#endif

    IncludeRegister("spike.h", &SpikeLibrarySetupFunc, &SpikeFunctions[0], NULL);
}

/* clean up space used by the include system */
void IncludeCleanup()
{
    struct IncludeLibrary *ThisInclude = IncludeLibList;
    struct IncludeLibrary *NextInclude;
    
    while (ThisInclude != NULL)
    {
        NextInclude = ThisInclude->NextLib;
        HeapFreeMem(ThisInclude);
        ThisInclude = NextInclude;
    }

    IncludeLibList = NULL;
}

/* register a new build-in include file */
void IncludeRegister(const char *IncludeName, void (*SetupFunction)(void), struct LibraryFunction *FuncList, const char *SetupCSource)
{
    struct IncludeLibrary *NewLib = HeapAllocMem(sizeof(struct IncludeLibrary));
    NewLib->IncludeName = TableStrRegister(IncludeName);
    NewLib->SetupFunction = SetupFunction;
    NewLib->FuncList = FuncList;
    NewLib->SetupCSource = SetupCSource;
    NewLib->NextLib = IncludeLibList;
    IncludeLibList = NewLib;
}

/* include all of the system headers */
void PicocIncludeAllSystemHeaders()
{
    struct IncludeLibrary *ThisInclude = IncludeLibList;
    
    for (; ThisInclude != NULL; ThisInclude = ThisInclude->NextLib)
        IncludeFile(ThisInclude->IncludeName);
}

/* include one of a number of predefined libraries, or perhaps an actual file */
void IncludeFile(char *FileName)
{
    struct IncludeLibrary *LInclude;
    
    /* scan for the include file name to see if it's in our list of predefined includes */
    for (LInclude = IncludeLibList; LInclude != NULL; LInclude = LInclude->NextLib)
    {
        if (strcmp(LInclude->IncludeName, FileName) == 0)
        {
            /* found it - protect against multiple inclusion */
            if (!VariableDefined(FileName))
            {
                VariableDefine(NULL, FileName, NULL, &VoidType, FALSE);
                
                /* run an extra startup function if there is one */
                if (LInclude->SetupFunction != NULL)
                    (*LInclude->SetupFunction)();
                
                /* parse the setup C source code - may define types etc. */
                if (LInclude->SetupCSource != NULL)
                    PicocParse(FileName, LInclude->SetupCSource, strlen(LInclude->SetupCSource), TRUE, TRUE, FALSE);
                
                /* set up the library functions */
                if (LInclude->FuncList != NULL)
                    LibraryAdd(&GlobalTable, FileName, LInclude->FuncList);
            }
            
            return;
        }
    }
    
    /* not a predefined file, read a real file */
    PicocPlatformScanFile(FileName);
}

#endif /* NO_HASH_INCLUDE */
