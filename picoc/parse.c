#include "picoc.h"
#include "interpreter.h"

/* a chunk of heap-allocated tokens we'll cleanup when we're done */
struct CleanupTokenNode
{
    void *Tokens;
    const char *SourceText;
    struct CleanupTokenNode *Next;
};

static struct CleanupTokenNode *CleanupTokenList = NULL;


/* deallocate any memory */
void ParseCleanup()
{
    while (CleanupTokenList != NULL)
    {
        struct CleanupTokenNode *Next = CleanupTokenList->Next;
        
        HeapFreeMem(CleanupTokenList->Tokens);
        if (CleanupTokenList->SourceText != NULL)
            HeapFreeMem((void *)CleanupTokenList->SourceText);
            
        HeapFreeMem(CleanupTokenList);
        CleanupTokenList = Next;
    }
}

/* parse a statement, but only run it if Condition is TRUE */
enum ParseResult ParseStatementMaybeRun(struct ParseState *Parser, int Condition, int CheckTrailingSemicolon)
{
    if (Parser->Mode != RunModeSkip && !Condition)
    {
        enum RunMode OldMode = Parser->Mode;
        int Result;
        Parser->Mode = RunModeSkip;
        Result = ParseStatement(Parser, CheckTrailingSemicolon);
        Parser->Mode = OldMode;
        return Result;
    }
    else
        return ParseStatement(Parser, CheckTrailingSemicolon);
}

/* count the number of parameters to a function or macro */
int ParseCountParams(struct ParseState *Parser)
{
    int ParamCount = 0;
    
    enum LexToken Token = LexGetToken(Parser, NULL, TRUE);
    if (Token != TokenCloseBracket && Token != TokenEOF)
    { 
        /* count the number of parameters */
        ParamCount++;
        while ((Token = LexGetToken(Parser, NULL, TRUE)) != TokenCloseBracket && Token != TokenEOF)
        { 
            if (Token == TokenComma)
                ParamCount++;
        } 
    }
    
    return ParamCount;
}

/* parse a function definition and store it for later */
struct Value *ParseFunctionDefinition(struct ParseState *Parser, struct ValueType *ReturnType, char *Identifier)
{
    struct ValueType *ParamType;
    char *ParamIdentifier;
    enum LexToken Token = TokenNone;
    struct ParseState ParamParser;
    struct Value *FuncValue;
    struct Value *OldFuncValue;
    struct ParseState FuncBody;
    int ParamCount = 0;

    if (TopStackFrame != NULL)
        ProgramFail(Parser, "nested function definitions are not allowed");
        
    LexGetToken(Parser, NULL, TRUE);  /* open bracket */
    ParserCopy(&ParamParser, Parser);
    ParamCount = ParseCountParams(Parser);
    if (ParamCount > PARAMETER_MAX)
        ProgramFail(Parser, "too many parameters");
    
    FuncValue = VariableAllocValueAndData(Parser, sizeof(struct FuncDef) + sizeof(struct ValueType *) * ParamCount + sizeof(const char *) * ParamCount, FALSE, NULL, TRUE);
    FuncValue->Typ = &FunctionType;
    FuncValue->Val->FuncDef.ReturnType = ReturnType;
    FuncValue->Val->FuncDef.NumParams = ParamCount;
    FuncValue->Val->FuncDef.VarArgs = FALSE;
    FuncValue->Val->FuncDef.ParamType = (struct ValueType **)((char *)FuncValue->Val + sizeof(struct FuncDef));
    FuncValue->Val->FuncDef.ParamName = (char **)((char *)FuncValue->Val->FuncDef.ParamType + sizeof(struct ValueType *) * ParamCount);
   
    for (ParamCount = 0; ParamCount < FuncValue->Val->FuncDef.NumParams; ParamCount++)
    { 
        /* harvest the parameters into the function definition */
        if (ParamCount == FuncValue->Val->FuncDef.NumParams-1 && LexGetToken(&ParamParser, NULL, FALSE) == TokenEllipsis)
        { 
            /* ellipsis at end */
            FuncValue->Val->FuncDef.NumParams--;
            FuncValue->Val->FuncDef.VarArgs = TRUE;
            break;
        }
        else
        { 
            /* add a parameter */
            TypeParse(&ParamParser, &ParamType, &ParamIdentifier, NULL);
            if (ParamType->Base == TypeVoid)
            {
                /* this isn't a real parameter at all - delete it */
                ParamCount--;
                FuncValue->Val->FuncDef.NumParams--;
            }
            else
            {
                FuncValue->Val->FuncDef.ParamType[ParamCount] = ParamType;
                FuncValue->Val->FuncDef.ParamName[ParamCount] = ParamIdentifier;
            }
        }
        
        Token = LexGetToken(&ParamParser, NULL, TRUE);
        if (Token != TokenComma && ParamCount < FuncValue->Val->FuncDef.NumParams-1)
            ProgramFail(&ParamParser, "comma expected");
    }
    
    if (FuncValue->Val->FuncDef.NumParams != 0 && Token != TokenCloseBracket && Token != TokenComma && Token != TokenEllipsis)
        ProgramFail(&ParamParser, "bad parameter");
    
    if (strcmp(Identifier, "main") == 0)
    {
        /* make sure it's int main() */
        if ( FuncValue->Val->FuncDef.ReturnType != &IntType &&
             FuncValue->Val->FuncDef.ReturnType != &VoidType )
            ProgramFail(Parser, "main() should return an int or void");

        if (FuncValue->Val->FuncDef.NumParams != 0 &&
             (FuncValue->Val->FuncDef.NumParams != 2 || FuncValue->Val->FuncDef.ParamType[0] != &IntType) )
            ProgramFail(Parser, "bad parameters to main()");
    }
    
    /* look for a function body */
    Token = LexGetToken(Parser, NULL, FALSE);
    if (Token == TokenSemicolon)
        LexGetToken(Parser, NULL, TRUE);    /* it's a prototype, absorb the trailing semicolon */
    else
    {
        /* it's a full function definition with a body */
        if (Token != TokenLeftBrace)
            ProgramFail(Parser, "bad function definition");
        
        ParserCopy(&FuncBody, Parser);
        if (ParseStatementMaybeRun(Parser, FALSE, TRUE) != ParseResultOk)
            ProgramFail(Parser, "function definition expected");

        FuncValue->Val->FuncDef.Body = FuncBody;
        FuncValue->Val->FuncDef.Body.Pos = LexCopyTokens(&FuncBody, Parser);

        /* is this function already in the global table? */
        if (TableGet(&GlobalTable, Identifier, &OldFuncValue, NULL, NULL, NULL))
        {
            if (OldFuncValue->Val->FuncDef.Body.Pos == NULL)
            {
                /* override an old function prototype */
                VariableFree(TableDelete(&GlobalTable, Identifier));
            }
            else
                ProgramFail(Parser, "'%s' is already defined", Identifier);
        }
    }

    if (!TableSet(&GlobalTable, Identifier, FuncValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos))
        ProgramFail(Parser, "'%s' is already defined", Identifier);
        
    return FuncValue;
}

/* assign an initial value to a variable */
void ParseDeclarationAssignment(struct ParseState *Parser, struct Value *NewVariable, int DoAssignment)
{
    struct Value *CValue;
    int ArrayIndex;
    enum LexToken Token = TokenComma;

    if (LexGetToken(Parser, NULL, FALSE) == TokenLeftBrace)
    {
        /* this is an array initialiser */
        LexGetToken(Parser, NULL, TRUE);
        
        for (ArrayIndex = 0; (Parser->Mode != RunModeRun && Token == TokenComma) || (Parser->Mode == RunModeRun && ArrayIndex < NewVariable->Typ->ArraySize); ArrayIndex++)
        {
            struct Value *ArrayElement = NULL;
            
            if (Token != TokenComma)
                ProgramFail(Parser, "comma expected");
                
            if (Parser->Mode == RunModeRun)
                ArrayElement = VariableAllocValueFromExistingData(Parser, NewVariable->Typ->FromType, (union AnyValue *)(&NewVariable->Val->ArrayMem[0] + TypeSize(NewVariable->Typ->FromType, 0, TRUE) * ArrayIndex), TRUE, NewVariable);
                
            if (!ExpressionParse(Parser, &CValue))
                ProgramFail(Parser, "expression expected");
                
            if (Parser->Mode == RunModeRun && DoAssignment)
            {
                ExpressionAssign(Parser, ArrayElement, CValue, FALSE, NULL, 0, FALSE);
                VariableStackPop(Parser, CValue);
                VariableStackPop(Parser, ArrayElement);
            }
            
            Token = LexGetToken(Parser, NULL, TRUE);
        }
        
        if (Token == TokenComma)
            Token = LexGetToken(Parser, NULL, TRUE);

        if (Token != TokenRightBrace)
            ProgramFail(Parser, "'}' expected");
    }
    else
    {
        /* this is a normal expression initialiser */
        if (!ExpressionParse(Parser, &CValue))
            ProgramFail(Parser, "expression expected");
            
        if (Parser->Mode == RunModeRun && DoAssignment)
        {
            ExpressionAssign(Parser, NewVariable, CValue, FALSE, NULL, 0, FALSE);
            VariableStackPop(Parser, CValue);
        }
    }
}

/* declare a variable or function */
int ParseDeclaration(struct ParseState *Parser, enum LexToken Token)
{
    char *Identifier;
    struct ValueType *BasicType;
    struct ValueType *Typ;
    struct Value *NewVariable = NULL;
    int IsStatic = FALSE;
    int FirstVisit = FALSE;

    TypeParseFront(Parser, &BasicType, &IsStatic);
    do
    {
        TypeParseIdentPart(Parser, BasicType, &Typ, &Identifier);
        if ((Token != TokenVoidType && Token != TokenStructType && Token != TokenUnionType && Token != TokenEnumType) && Identifier == StrEmpty)
            ProgramFail(Parser, "identifier expected");
            
        if (Identifier != StrEmpty)
        {
            /* handle function definitions */
            if (LexGetToken(Parser, NULL, FALSE) == TokenOpenBracket)
            {
                ParseFunctionDefinition(Parser, Typ, Identifier);
                return FALSE;
            }
            else
            {
                if (Typ == &VoidType && Identifier != StrEmpty)
                    ProgramFail(Parser, "can't define a void variable");
                    
                if (Parser->Mode == RunModeRun || Parser->Mode == RunModeGoto)
                    NewVariable = VariableDefineButIgnoreIdentical(Parser, Identifier, Typ, IsStatic, &FirstVisit);
                
                if (LexGetToken(Parser, NULL, FALSE) == TokenAssign)
                {
                    /* we're assigning an initial value */
                    LexGetToken(Parser, NULL, TRUE);
                    ParseDeclarationAssignment(Parser, NewVariable, !IsStatic || FirstVisit);
                }
            }
        }
        
        Token = LexGetToken(Parser, NULL, FALSE);
        if (Token == TokenComma)
            LexGetToken(Parser, NULL, TRUE);
            
    } while (Token == TokenComma);
    
    return TRUE;
}

/* parse a #define macro definition and store it for later */
void ParseMacroDefinition(struct ParseState *Parser)
{
    struct Value *MacroName;
    char *MacroNameStr;
    struct Value *ParamName;
    struct Value *MacroValue;

    if (LexGetToken(Parser, &MacroName, TRUE) != TokenIdentifier)
        ProgramFail(Parser, "identifier expected");
    
    MacroNameStr = MacroName->Val->Identifier;
    
    if (LexRawPeekToken(Parser) == TokenOpenMacroBracket)
    {
        /* it's a parameterised macro, read the parameters */
        enum LexToken Token = LexGetToken(Parser, NULL, TRUE);
        struct ParseState ParamParser;
        int NumParams;
        int ParamCount = 0;
        
        ParserCopy(&ParamParser, Parser);
        NumParams = ParseCountParams(&ParamParser);
        MacroValue = VariableAllocValueAndData(Parser, sizeof(struct MacroDef) + sizeof(const char *) * NumParams, FALSE, NULL, TRUE);
        MacroValue->Val->MacroDef.NumParams = NumParams;
        MacroValue->Val->MacroDef.ParamName = (char **)((char *)MacroValue->Val + sizeof(struct MacroDef));

        Token = LexGetToken(Parser, &ParamName, TRUE);
        
        while (Token == TokenIdentifier)
        {
            /* store a parameter name */
            MacroValue->Val->MacroDef.ParamName[ParamCount++] = ParamName->Val->Identifier;
            
            /* get the trailing comma */
            Token = LexGetToken(Parser, NULL, TRUE);
            if (Token == TokenComma)
                Token = LexGetToken(Parser, &ParamName, TRUE);
                
            else if (Token != TokenCloseBracket)
                ProgramFail(Parser, "comma expected");
        }
        
        if (Token != TokenCloseBracket)
            ProgramFail(Parser, "close bracket expected");
    }
    else
    {
        /* allocate a simple unparameterised macro */
        MacroValue = VariableAllocValueAndData(Parser, sizeof(struct MacroDef), FALSE, NULL, TRUE);
        MacroValue->Val->MacroDef.NumParams = 0;
    }
    
    /* copy the body of the macro to execute later */
    ParserCopy(&MacroValue->Val->MacroDef.Body, Parser);
    MacroValue->Typ = &MacroType;
    LexToEndOfLine(Parser);
    MacroValue->Val->MacroDef.Body.Pos = LexCopyTokens(&MacroValue->Val->MacroDef.Body, Parser);
    
    if (!TableSet(&GlobalTable, MacroNameStr, MacroValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos))
        ProgramFail(Parser, "'%s' is already defined", MacroNameStr);
}

/* copy the entire parser state */
void ParserCopy(struct ParseState *To, struct ParseState *From)
{
    memcpy((void *)To, (void *)From, sizeof(*To));
}

/* copy where we're at in the parsing */
void ParserCopyPos(struct ParseState *To, struct ParseState *From)
{
    To->Pos = From->Pos;
    To->Line = From->Line;
    To->HashIfLevel = From->HashIfLevel;
    To->HashIfEvaluateToLevel = From->HashIfEvaluateToLevel;
    To->CharacterPos = From->CharacterPos;
}

/* parse a "for" statement */
void ParseFor(struct ParseState *Parser)
{
    int Condition;
    struct ParseState PreConditional;
    struct ParseState PreIncrement;
    struct ParseState PreStatement;
    struct ParseState After;

    if (LexGetToken(Parser, NULL, TRUE) != TokenOpenBracket)
        ProgramFail(Parser, "'(' expected");
                        
    if (ParseStatement(Parser, TRUE) != ParseResultOk)
        ProgramFail(Parser, "statement expected");
    
    ParserCopyPos(&PreConditional, Parser);
    if (LexGetToken(Parser, NULL, FALSE) == TokenSemicolon)
        Condition = TRUE;
    else
        Condition = ExpressionParseInt(Parser);
    
    if (LexGetToken(Parser, NULL, TRUE) != TokenSemicolon)
        ProgramFail(Parser, "';' expected");
    
    ParserCopyPos(&PreIncrement, Parser);
    ParseStatementMaybeRun(Parser, FALSE, FALSE);
    
    if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
        ProgramFail(Parser, "')' expected");
    
    ParserCopyPos(&PreStatement, Parser);
    if (ParseStatementMaybeRun(Parser, Condition, TRUE) != ParseResultOk)
        ProgramFail(Parser, "statement expected");
    
    if (Parser->Mode == RunModeContinue)
        Parser->Mode = RunModeRun;
        
    ParserCopyPos(&After, Parser);
        
    while (Condition && Parser->Mode == RunModeRun)
    {
        ParserCopyPos(Parser, &PreIncrement);
        ParseStatement(Parser, FALSE);
                        
        ParserCopyPos(Parser, &PreConditional);
        if (LexGetToken(Parser, NULL, FALSE) == TokenSemicolon)
            Condition = TRUE;
        else
            Condition = ExpressionParseInt(Parser);
        
        if (Condition)
        {
            ParserCopyPos(Parser, &PreStatement);
            ParseStatement(Parser, TRUE);
            
            if (Parser->Mode == RunModeContinue)
                Parser->Mode = RunModeRun;                
        }
    }
    
    if (Parser->Mode == RunModeBreak)
        Parser->Mode = RunModeRun;
        
    ParserCopyPos(Parser, &After);
}

/* parse a block of code and return what mode it returned in */
enum RunMode ParseBlock(struct ParseState *Parser, int AbsorbOpenBrace, int Condition)
{
    if (AbsorbOpenBrace && LexGetToken(Parser, NULL, TRUE) != TokenLeftBrace)
        ProgramFail(Parser, "'{' expected");
    
    if (Parser->Mode == RunModeSkip || !Condition)
    { 
        /* condition failed - skip this block instead */
        enum RunMode OldMode = Parser->Mode;
        Parser->Mode = RunModeSkip;
        while (ParseStatement(Parser, TRUE) == ParseResultOk)
        {}
        Parser->Mode = OldMode;
    }
    else
    { 
        /* just run it in its current mode */
        while (ParseStatement(Parser, TRUE) == ParseResultOk)
        {}
    }
    
    if (LexGetToken(Parser, NULL, TRUE) != TokenRightBrace)
        ProgramFail(Parser, "'}' expected");
        
    return Parser->Mode;
}

/* parse a typedef declaration */
void ParseTypedef(struct ParseState *Parser)
{
    struct ValueType *Typ;
    struct ValueType **TypPtr;
    char *TypeName;
    struct Value InitValue;
    
    TypeParse(Parser, &Typ, &TypeName, NULL);
    
    if (Parser->Mode == RunModeRun)
    {
        TypPtr = &Typ;
        InitValue.Typ = &TypeType;
        InitValue.Val = (union AnyValue *)TypPtr;
        VariableDefine(Parser, TypeName, &InitValue, NULL, FALSE);
    }
}

/* parse a statement */
enum ParseResult ParseStatement(struct ParseState *Parser, int CheckTrailingSemicolon)
{
    struct Value *CValue;
    struct Value *LexerValue;
    struct Value *VarValue;
    int Condition;
    struct ParseState PreState;
    enum LexToken Token;
    
    ParserCopy(&PreState, Parser);
    Token = LexGetToken(Parser, &LexerValue, TRUE);
    
    switch (Token)
    {
        case TokenEOF:
            return ParseResultEOF;
            
        case TokenIdentifier:
            /* might be a typedef-typed variable declaration or it might be an expression */
            if (VariableDefined(LexerValue->Val->Identifier))
            {
                VariableGet(Parser, LexerValue->Val->Identifier, &VarValue);
                if (VarValue->Typ->Base == Type_Type)
                {
                    *Parser = PreState;
                    ParseDeclaration(Parser, Token);
                    break;
                }
            }
            else
            {
                /* it might be a goto label */
                enum LexToken NextToken = LexGetToken(Parser, NULL, FALSE);
                if (NextToken == TokenColon)
                {
                    /* declare the identifier as a goto label */
                    LexGetToken(Parser, NULL, TRUE);
                    if (Parser->Mode == RunModeGoto && LexerValue->Val->Identifier == Parser->SearchGotoLabel)
                        Parser->Mode = RunModeRun;
        
                    CheckTrailingSemicolon = FALSE;
                    break;
                }
            }
            /* else fallthrough to expression */
            
        case TokenAsterisk: 
        case TokenAmpersand: 
        case TokenIncrement: 
        case TokenDecrement: 
        case TokenOpenBracket: 
            *Parser = PreState;
            ExpressionParse(Parser, &CValue);
            if (Parser->Mode == RunModeRun) 
                VariableStackPop(Parser, CValue);
            break;
            
        case TokenLeftBrace:
            ParseBlock(Parser, FALSE, TRUE);
            CheckTrailingSemicolon = FALSE;
            break;
            
        case TokenIf:
            if (LexGetToken(Parser, NULL, TRUE) != TokenOpenBracket)
                ProgramFail(Parser, "'(' expected");
                
            Condition = ExpressionParseInt(Parser);
            
            if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
                ProgramFail(Parser, "')' expected");

            if (ParseStatementMaybeRun(Parser, Condition, TRUE) != ParseResultOk)
                ProgramFail(Parser, "statement expected");
            
            if (LexGetToken(Parser, NULL, FALSE) == TokenElse)
            {
                LexGetToken(Parser, NULL, TRUE);
                if (ParseStatementMaybeRun(Parser, !Condition, TRUE) != ParseResultOk)
                    ProgramFail(Parser, "statement expected");
            }
            CheckTrailingSemicolon = FALSE;
            break;
        
        case TokenWhile:
            {
                struct ParseState PreConditional;
                enum RunMode PreMode = Parser->Mode;

                if (LexGetToken(Parser, NULL, TRUE) != TokenOpenBracket)
                    ProgramFail(Parser, "'(' expected");
                    
                ParserCopyPos(&PreConditional, Parser);
                do
                {
                    ParserCopyPos(Parser, &PreConditional);
                    Condition = ExpressionParseInt(Parser);
                    if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
                        ProgramFail(Parser, "')' expected");
                    
                    if (ParseStatementMaybeRun(Parser, Condition, TRUE) != ParseResultOk)
                        ProgramFail(Parser, "statement expected");
                    
                    if (Parser->Mode == RunModeContinue)
                        Parser->Mode = PreMode;
                    
                } while (Parser->Mode == RunModeRun && Condition);
                
                if (Parser->Mode == RunModeBreak)
                    Parser->Mode = PreMode;

                CheckTrailingSemicolon = FALSE;
            }
            break;
                
        case TokenDo:
            {
                struct ParseState PreStatement;
                enum RunMode PreMode = Parser->Mode;
                ParserCopyPos(&PreStatement, Parser);
                do
                {
                    ParserCopyPos(Parser, &PreStatement);
                    if (ParseStatement(Parser, TRUE) != ParseResultOk)
                        ProgramFail(Parser, "statement expected");
                
                    if (Parser->Mode == RunModeContinue)
                        Parser->Mode = PreMode;

                    if (LexGetToken(Parser, NULL, TRUE) != TokenWhile)
                        ProgramFail(Parser, "'while' expected");
                    
                    if (LexGetToken(Parser, NULL, TRUE) != TokenOpenBracket)
                        ProgramFail(Parser, "'(' expected");
                        
                    Condition = ExpressionParseInt(Parser);
                    if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
                        ProgramFail(Parser, "')' expected");
                    
                } while (Condition && Parser->Mode == RunModeRun);           
                
                if (Parser->Mode == RunModeBreak)
                    Parser->Mode = PreMode;
            }
            break;
                
        case TokenFor:
            ParseFor(Parser);
            CheckTrailingSemicolon = FALSE;
            break;

        case TokenSemicolon: 
            CheckTrailingSemicolon = FALSE; 
            break;

        case TokenIntType:
        case TokenShortType:
        case TokenCharType:
        case TokenLongType:
        case TokenFloatType:
        case TokenDoubleType:
        case TokenVoidType:
        case TokenStructType:
        case TokenUnionType:
        case TokenEnumType:
        case TokenSignedType:
        case TokenUnsignedType:
        case TokenStaticType:
        case TokenAutoType:
        case TokenRegisterType:
        case TokenExternType:
            *Parser = PreState;
            CheckTrailingSemicolon = ParseDeclaration(Parser, Token);
            break;
        
        case TokenHashDefine:
            ParseMacroDefinition(Parser);
            CheckTrailingSemicolon = FALSE;
            break;
            
#ifndef NO_HASH_INCLUDE
        case TokenHashInclude:
            if (LexGetToken(Parser, &LexerValue, TRUE) != TokenStringConstant)
                ProgramFail(Parser, "\"filename.h\" expected");
            
            IncludeFile((char *)LexerValue->Val->Pointer);
            CheckTrailingSemicolon = FALSE;
            break;
#endif

        case TokenSwitch:
            if (LexGetToken(Parser, NULL, TRUE) != TokenOpenBracket)
                ProgramFail(Parser, "'(' expected");
                
            Condition = ExpressionParseInt(Parser);
            
            if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
                ProgramFail(Parser, "')' expected");
            
            if (LexGetToken(Parser, NULL, FALSE) != TokenLeftBrace)
                ProgramFail(Parser, "'{' expected");
            
            { 
                /* new block so we can store parser state */
                enum RunMode OldMode = Parser->Mode;
                int OldSearchLabel = Parser->SearchLabel;
                Parser->Mode = RunModeCaseSearch;
                Parser->SearchLabel = Condition;
                
                ParseBlock(Parser, TRUE, OldMode != RunModeSkip);
                
                if (Parser->Mode != RunModeReturn)
                    Parser->Mode = OldMode;

                Parser->SearchLabel = OldSearchLabel;
            }

            CheckTrailingSemicolon = FALSE;
            break;

        case TokenCase:
            if (Parser->Mode == RunModeCaseSearch)
            {
                Parser->Mode = RunModeRun;
                Condition = ExpressionParseInt(Parser);
                Parser->Mode = RunModeCaseSearch;
            }
            else
                Condition = ExpressionParseInt(Parser);
                
            if (LexGetToken(Parser, NULL, TRUE) != TokenColon)
                ProgramFail(Parser, "':' expected");
            
            if (Parser->Mode == RunModeCaseSearch && Condition == Parser->SearchLabel)
                Parser->Mode = RunModeRun;

            CheckTrailingSemicolon = FALSE;
            break;
            
        case TokenDefault:
            if (LexGetToken(Parser, NULL, TRUE) != TokenColon)
                ProgramFail(Parser, "':' expected");
            
            if (Parser->Mode == RunModeCaseSearch)
                Parser->Mode = RunModeRun;
                
            CheckTrailingSemicolon = FALSE;
            break;

        case TokenBreak:
            if (Parser->Mode == RunModeRun)
                Parser->Mode = RunModeBreak;
            break;
            
        case TokenContinue:
            if (Parser->Mode == RunModeRun)
                Parser->Mode = RunModeContinue;
            break;
            
        case TokenReturn:
            if (Parser->Mode == RunModeRun)
            {
                if (TopStackFrame->ReturnValue->Typ->Base != TypeVoid)
                {
                    if (!ExpressionParse(Parser, &CValue))
                        ProgramFail(Parser, "value required in return");
                    
                    ExpressionAssign(Parser, TopStackFrame->ReturnValue, CValue, TRUE, NULL, 0, FALSE);
                    VariableStackPop(Parser, CValue);
                }
                else
                {
                    if (ExpressionParse(Parser, &CValue))
                        ProgramFail(Parser, "value in return from a void function");                    
                }
                
                Parser->Mode = RunModeReturn;
            }
            else
                ExpressionParse(Parser, &CValue);
            break;

        case TokenTypedef:
            ParseTypedef(Parser);
            break;
            
        case TokenGoto:
            if (LexGetToken(Parser, &LexerValue, TRUE) != TokenIdentifier)
                ProgramFail(Parser, "identifier expected");
            
            if (Parser->Mode == RunModeRun)
            { 
                /* start scanning for the goto label */
                Parser->SearchGotoLabel = LexerValue->Val->Identifier;
                Parser->Mode = RunModeGoto;
            }
            break;
                
        case TokenDelete:
        {
            /* try it as a function or variable name to delete */
            if (LexGetToken(Parser, &LexerValue, TRUE) != TokenIdentifier)
                ProgramFail(Parser, "identifier expected");
                
            if (Parser->Mode == RunModeRun)
            { 
                /* delete this variable or function */
                CValue = TableDelete(&GlobalTable, LexerValue->Val->Identifier);

                if (CValue == NULL)
                    ProgramFail(Parser, "'%s' is not defined", LexerValue->Val->Identifier);
                
                VariableFree(CValue);
            }
            break;
        }
        
        default:
            *Parser = PreState;
            return ParseResultError;
    }
    
    if (CheckTrailingSemicolon)
    {
        if (LexGetToken(Parser, NULL, TRUE) != TokenSemicolon)
            ProgramFail(Parser, "';' expected");
    }
    
    return ParseResultOk;
}

/* quick scan a source file for definitions */
void PicocParse(const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource)
{
    struct ParseState Parser;
    enum ParseResult Ok;
    struct CleanupTokenNode *NewCleanupNode;
    
    void *Tokens = LexAnalyse(FileName, Source, SourceLen, NULL);
    
    /* allocate a cleanup node so we can clean up the tokens later */
    if (!CleanupNow)
    {
        NewCleanupNode = HeapAllocMem(sizeof(struct CleanupTokenNode));
        if (NewCleanupNode == NULL)
            ProgramFail(NULL, "out of memory");
        
        NewCleanupNode->Tokens = Tokens;
        if (CleanupSource)
            NewCleanupNode->SourceText = Source;
        else
            NewCleanupNode->SourceText = NULL;
            
        NewCleanupNode->Next = CleanupTokenList;
        CleanupTokenList = NewCleanupNode;
    }
    
    /* do the parsing */
    LexInitParser(&Parser, Source, Tokens, FileName, RunIt);

    do {
        Ok = ParseStatement(&Parser, TRUE);
    } while (Ok == ParseResultOk);
    
    if (Ok == ParseResultError)
        ProgramFail(&Parser, "parse error");
    
    /* clean up */
    if (CleanupNow)
        HeapFreeMem(Tokens);
}

/* parse interactively */
void PicocParseInteractive()
{
    struct ParseState Parser;
    enum ParseResult Ok;
    
    PlatformPrintf(INTERACTIVE_PROMPT_START);
    LexInitParser(&Parser, NULL, NULL, StrEmpty, TRUE);
    PicocPlatformSetExitPoint();
    LexInteractiveClear(&Parser);

    do
    {
        LexInteractiveStatementPrompt();
        Ok = ParseStatement(&Parser, TRUE);
        LexInteractiveCompleted(&Parser);
        
    } while (Ok == ParseResultOk);
    
    if (Ok == ParseResultError)
        ProgramFail(&Parser, "parse error");
    
    PlatformPrintf("\n");
}
