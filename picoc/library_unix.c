#include "interpreter.h"

void UnixSetupFunc()
{    
}

void Ctest (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    printf("test(%d)\n", Param[0]->Val->Integer);
    Param[0]->Val->Integer = 1234;
}

void Clineno (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    ReturnValue->Val->Integer = Parser->Line;
}

/* list of all library functions and their prototypes */
struct LibraryFunction UnixFunctions[] =
{
    { Ctest,        "void test(int);" },
    { Clineno,      "int lineno();" },
    { NULL,         NULL }
};

void PlatformLibraryInit()
{
    IncludeRegister("picoc_unix.h", &UnixSetupFunc, &UnixFunctions[0], NULL);
}
