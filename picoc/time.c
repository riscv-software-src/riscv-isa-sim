/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <time.h>
#include "interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int CLOCKS_PER_SECValue = CLOCKS_PER_SEC;

#ifdef CLK_PER_SEC
static int CLK_PER_SECValue = CLK_PER_SEC;
#endif

#ifdef CLK_TCK
static int CLK_TCKValue = CLK_TCK;
#endif

void StdAsctime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = asctime(Param[0]->Val->Pointer);
}

void StdClock(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = clock();
}

void StdCtime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = ctime(Param[0]->Val->Pointer);
}

#ifndef NO_FP
void StdDifftime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = difftime((time_t)Param[0]->Val->Integer, Param[1]->Val->Integer);
}
#endif

void StdGmtime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = gmtime(Param[0]->Val->Pointer);
}

void StdGmtime_r(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = gmtime_r(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StdLocaltime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = localtime(Param[0]->Val->Pointer);
}

void StdMktime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mktime(Param[0]->Val->Pointer);
}

void StdTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = time(Param[0]->Val->Pointer);
}

void StdStrftime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strftime(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Pointer, Param[3]->Val->Pointer);
}

void StdStrptime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	  extern char *strptime(const char *s, const char *format, struct tm *tm);
	  
    ReturnValue->Val->Pointer = strptime(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Pointer);
}

void StdTimegm(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = timegm(Param[0]->Val->Pointer);
}

/* handy structure definitions */
const char StdTimeDefs[] = "\
typedef int time_t; \
typedef int clock_t;\
";

/* all string.h functions */
struct LibraryFunction StdTimeFunctions[] =
{
    { StdAsctime,       "char *asctime(struct tm *);" },
    { StdClock,         "time_t clock();" },
    { StdCtime,         "char *ctime(int *);" },
#ifndef NO_FP
    { StdDifftime,      "double difftime(int, int);" },
#endif
    { StdGmtime,        "struct tm *gmtime(int *);" },
    { StdGmtime_r,      "struct tm *gmtime_r(int *, struct tm *);" },
    { StdLocaltime,     "struct tm *localtime(int *);" },
    { StdMktime,        "int mktime(struct tm *ptm);" },
    { StdTime,          "int time(int *);" },
    { StdStrftime,      "int strftime(char *, int, char *, struct tm *);" },
    { StdStrptime,      "char *strptime(char *, char *, struct tm *);" },
    { StdTimegm,        "int timegm(struct tm *);" },
    { NULL,             NULL }
};


/* creates various system-dependent definitions */
void StdTimeSetupFunc(void)
{
    /* make a "struct tm" which is the same size as a native tm structure */
    TypeCreateOpaqueStruct(NULL, TableStrRegister("tm"), sizeof(struct tm));
    
    /* define CLK_PER_SEC etc. */
    VariableDefinePlatformVar(NULL, "CLOCKS_PER_SEC", &IntType, (union AnyValue *)&CLOCKS_PER_SECValue, FALSE);
#ifdef CLK_PER_SEC
    VariableDefinePlatformVar(NULL, "CLK_PER_SEC", &IntType, (union AnyValue *)&CLK_PER_SECValue, FALSE);
#endif
#ifdef CLK_TCK
    VariableDefinePlatformVar(NULL, "CLK_TCK", &IntType, (union AnyValue *)&CLK_TCKValue, FALSE);
#endif
}

#endif /* !BUILTIN_MINI_STDLIB */
