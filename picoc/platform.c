#include "picoc.h"
#include "interpreter.h"

/* the value passed to exit() */
int PicocExitValue = 0;

/* initialise everything */
void PicocInitialise(int StackSize)
{
    BasicIOInit();
    HeapInit(StackSize);
    TableInit();
    VariableInit();
    LexInit();
    TypeInit();
#ifndef NO_HASH_INCLUDE
    IncludeInit();
#endif
    LibraryInit();
#ifdef BUILTIN_MINI_STDLIB
    LibraryAdd(&GlobalTable, "c library", &CLibrary[0]);
    CLibraryInit();
#endif
    PlatformLibraryInit();
}

/* free memory */
void PicocCleanup()
{
    PlatformCleanup();
#ifndef NO_HASH_INCLUDE
    IncludeCleanup();
#endif
    ParseCleanup();
    LexCleanup();
    VariableCleanup();
    TypeCleanup();
    TableStrFree();
    HeapCleanup();
}

/* platform-dependent code for running programs */
#ifdef UNIX_HOST

#define CALL_MAIN_NO_ARGS_RETURN_VOID "main();"
#define CALL_MAIN_WITH_ARGS_RETURN_VOID "main(__argc,__argv);"
#define CALL_MAIN_NO_ARGS_RETURN_INT "__exit_value = main();"
#define CALL_MAIN_WITH_ARGS_RETURN_INT "__exit_value = main(__argc,__argv);"

void PicocCallMain(int argc, char **argv)
{
    /* check if the program wants arguments */
    struct Value *FuncValue = NULL;

    if (!VariableDefined(TableStrRegister("main")))
        ProgramFail(NULL, "main() is not defined");
        
    VariableGet(NULL, TableStrRegister("main"), &FuncValue);
    if (FuncValue->Typ->Base != TypeFunction)
        ProgramFail(NULL, "main is not a function - can't call it");

    if (FuncValue->Val->FuncDef.NumParams != 0)
    {
        /* define the arguments */
        VariableDefinePlatformVar(NULL, "__argc", &IntType, (union AnyValue *)&argc, FALSE);
        VariableDefinePlatformVar(NULL, "__argv", CharPtrPtrType, (union AnyValue *)&argv, FALSE);
    }

    if (FuncValue->Val->FuncDef.ReturnType == &VoidType)
    {
        if (FuncValue->Val->FuncDef.NumParams == 0)
            PicocParse("startup", CALL_MAIN_NO_ARGS_RETURN_VOID, strlen(CALL_MAIN_NO_ARGS_RETURN_VOID), TRUE, TRUE, FALSE);
        else
            PicocParse("startup", CALL_MAIN_WITH_ARGS_RETURN_VOID, strlen(CALL_MAIN_WITH_ARGS_RETURN_VOID), TRUE, TRUE, FALSE);
    }
    else
    {
        VariableDefinePlatformVar(NULL, "__exit_value", &IntType, (union AnyValue *)&PicocExitValue, TRUE);
    
        if (FuncValue->Val->FuncDef.NumParams == 0)
            PicocParse("startup", CALL_MAIN_NO_ARGS_RETURN_INT, strlen(CALL_MAIN_NO_ARGS_RETURN_INT), TRUE, TRUE, FALSE);
        else
            PicocParse("startup", CALL_MAIN_WITH_ARGS_RETURN_INT, strlen(CALL_MAIN_WITH_ARGS_RETURN_INT), TRUE, TRUE, FALSE);
    }
}
#endif

void PrintSourceTextErrorLine(const char *FileName, const char *SourceText, int Line, int CharacterPos)
{
    int LineCount;
    const char *LinePos;
    const char *CPos;
    int CCount;
    
    if (SourceText != NULL)
    {
        /* find the source line */
        for (LinePos = SourceText, LineCount = 1; *LinePos != '\0' && LineCount < Line; LinePos++)
        {
            if (*LinePos == '\n')
                LineCount++;
        }
        
        /* display the line */
        for (CPos = LinePos; *CPos != '\n' && *CPos != '\0'; CPos++)
            PrintCh(*CPos, CStdOut);
        PrintCh('\n', CStdOut);
        
        /* display the error position */
        for (CPos = LinePos, CCount = 0; *CPos != '\n' && *CPos != '\0' && (CCount < CharacterPos || *CPos == ' '); CPos++, CCount++)
        {
            if (*CPos == '\t')
                PrintCh('\t', CStdOut);
            else
                PrintCh(' ', CStdOut);
        }
    }
    else
    {
        /* assume we're in interactive mode - try to make the arrow match up with the input text */
        for (CCount = 0; CCount < CharacterPos + strlen(INTERACTIVE_PROMPT_STATEMENT); CCount++)
            PrintCh(' ', CStdOut);
    }
    PlatformPrintf("^\n%s:%d: ", FileName, Line, CharacterPos);
    
}

/* display the source line and line number to identify an error */
void PlatformErrorPrefix(struct ParseState *Parser)
{
    if (Parser != NULL)
        PrintSourceTextErrorLine(Parser->FileName, Parser->SourceText, Parser->Line, Parser->CharacterPos);
}

/* exit with a message */
void ProgramFail(struct ParseState *Parser, const char *Message, ...)
{
    va_list Args;

    PlatformErrorPrefix(Parser);
    va_start(Args, Message);
    PlatformVPrintf(Message, Args);
    va_end(Args);
    PlatformPrintf("\n");
    PlatformExit(1);
}

/* like ProgramFail() but gives descriptive error messages for assignment */
void AssignFail(struct ParseState *Parser, const char *Format, struct ValueType *Type1, struct ValueType *Type2, int Num1, int Num2, const char *FuncName, int ParamNo)
{
    PlatformErrorPrefix(Parser);
    PlatformPrintf("can't %s ", (FuncName == NULL) ? "assign" : "set");   
        
    if (Type1 != NULL)
        PlatformPrintf(Format, Type1, Type2);
    else
        PlatformPrintf(Format, Num1, Num2);
    
    if (FuncName != NULL)
        PlatformPrintf(" in argument %d of call to %s()", ParamNo, FuncName);
        
    ProgramFail(NULL, "");
}

/* exit lexing with a message */
void LexFail(struct LexState *Lexer, const char *Message, ...)
{
    va_list Args;

    PrintSourceTextErrorLine(Lexer->FileName, Lexer->SourceText, Lexer->Line, Lexer->CharacterPos);
    va_start(Args, Message);
    PlatformVPrintf(Message, Args);
    va_end(Args);
    PlatformPrintf("\n");
    PlatformExit(1);
}

/* printf for compiler error reporting */
void PlatformPrintf(const char *Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    PlatformVPrintf(Format, Args);
    va_end(Args);
}

void PlatformVPrintf(const char *Format, va_list Args)
{
    const char *FPos;
    
    for (FPos = Format; *FPos != '\0'; FPos++)
    {
        if (*FPos == '%')
        {
            FPos++;
            switch (*FPos)
            {
            case 's': PrintStr(va_arg(Args, char *), CStdOut); break;
            case 'd': PrintSimpleInt(va_arg(Args, int), CStdOut); break;
            case 'c': PrintCh(va_arg(Args, int), CStdOut); break;
            case 't': PrintType(va_arg(Args, struct ValueType *), CStdOut); break;
#ifndef NO_FP
            case 'f': PrintFP(va_arg(Args, double), CStdOut); break;
#endif
            case '%': PrintCh('%', CStdOut); break;
            case '\0': FPos--; break;
            }
        }
        else
            PrintCh(*FPos, CStdOut);
    }
}

/* make a new temporary name. takes a static buffer of char [7] as a parameter. should be initialised to "XX0000"
 * where XX can be any characters */
char *PlatformMakeTempName(char *TempNameBuffer)
{
    int CPos = 5;
    
    while (CPos > 1)
    {
        if (TempNameBuffer[CPos] < '9')
        {
            TempNameBuffer[CPos]++;
            return TableStrRegister(TempNameBuffer);
        }
        else
        {
            TempNameBuffer[CPos] = '0';
            CPos--;
        }
    }

    return TableStrRegister(TempNameBuffer);
}
