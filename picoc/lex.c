#include "interpreter.h"

#ifdef NO_CTYPE
#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#endif
#define isCidstart(c) (isalpha(c) || (c)=='_' || (c)=='#')
#define isCident(c) (isalnum(c) || (c)=='_')

#define IS_HEX_ALPHA_DIGIT(c) (((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define IS_BASE_DIGIT(c,b) (((c) >= '0' && (c) < '0' + (((b)<10)?(b):10)) || (((b) > 10) ? IS_HEX_ALPHA_DIGIT(c) : FALSE))
#define GET_BASE_DIGIT(c) (((c) <= '9') ? ((c) - '0') : (((c) <= 'F') ? ((c) - 'A' + 10) : ((c) - 'a' + 10)))

#define NEXTIS(c,x,y) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else GotToken = (y); }
#define NEXTIS3(c,x,d,y,z) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else NEXTIS(d,y,z) }
#define NEXTIS4(c,x,d,y,e,z,a) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else NEXTIS3(d,y,e,z,a) }
#define NEXTIS3PLUS(c,x,d,y,e,z,a) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else if (NextChar == (d)) { if (Lexer->Pos[1] == (e)) { LEXER_INCN(Lexer, 2); GotToken = (z); } else { LEXER_INC(Lexer); GotToken = (y); } } else GotToken = (a); }
#define NEXTISEXACTLY3(c,d,y,z) { if (NextChar == (c) && Lexer->Pos[1] == (d)) { LEXER_INCN(Lexer, 2); GotToken = (y); } else GotToken = (z); }

#define LEXER_INC(l) ( (l)->Pos++, (l)->CharacterPos++ )
#define LEXER_INCN(l, n) ( (l)->Pos+=(n), (l)->CharacterPos+=(n) )
#define TOKEN_DATA_OFFSET 2

#define MAX_CHAR_VALUE 255      /* maximum value which can be represented by a "char" data type */

static union AnyValue LexAnyValue;
static struct Value LexValue = { TypeVoid, &LexAnyValue, FALSE, FALSE };

struct ReservedWord
{
    const char *Word;
    enum LexToken Token;
    const char *SharedWord; /* word stored in shared string space */
};

static struct ReservedWord ReservedWords[] =
{
    { "#define", TokenHashDefine, NULL },
    { "#else", TokenHashElse, NULL },
    { "#endif", TokenHashEndif, NULL },
    { "#if", TokenHashIf, NULL },
    { "#ifdef", TokenHashIfdef, NULL },
    { "#ifndef", TokenHashIfndef, NULL },
    { "#include", TokenHashInclude, NULL },
    { "auto", TokenAutoType, NULL },
    { "break", TokenBreak, NULL },
    { "case", TokenCase, NULL },
    { "char", TokenCharType, NULL },
    { "continue", TokenContinue, NULL },
    { "default", TokenDefault, NULL },
    { "delete", TokenDelete, NULL },
    { "do", TokenDo, NULL },
#ifndef NO_FP
    { "double", TokenDoubleType, NULL },
#endif
    { "else", TokenElse, NULL },
    { "enum", TokenEnumType, NULL },
    { "extern", TokenExternType, NULL },
#ifndef NO_FP
    { "float", TokenFloatType, NULL },
#endif
    { "for", TokenFor, NULL },
    { "goto", TokenGoto, NULL },
    { "if", TokenIf, NULL },
    { "int", TokenIntType, NULL },
    { "long", TokenLongType, NULL },
    { "new", TokenNew, NULL },
    { "register", TokenRegisterType, NULL },
    { "return", TokenReturn, NULL },
    { "short", TokenShortType, NULL },
    { "signed", TokenSignedType, NULL },
    { "sizeof", TokenSizeof, NULL },
    { "static", TokenStaticType, NULL },
    { "struct", TokenStructType, NULL },
    { "switch", TokenSwitch, NULL },
    { "typedef", TokenTypedef, NULL },
    { "union", TokenUnionType, NULL },
    { "unsigned", TokenUnsignedType, NULL },
    { "void", TokenVoidType, NULL },
    { "while", TokenWhile, NULL }
};

/* linked list of tokens used in interactive mode */
struct TokenLine
{
    struct TokenLine *Next;
    unsigned char *Tokens;
    int NumBytes;
};

static struct TokenLine *InteractiveHead = NULL;
static struct TokenLine *InteractiveTail = NULL;
static struct TokenLine *InteractiveCurrentLine = NULL;
static int LexUseStatementPrompt = FALSE;


/* initialise the lexer */
void LexInit()
{
    int Count;
    
    for (Count = 0; Count < sizeof(ReservedWords) / sizeof(struct ReservedWord); Count++)
        ReservedWords[Count].SharedWord = TableStrRegister(ReservedWords[Count].Word);
}

/* deallocate */
void LexCleanup()
{
    LexInteractiveClear(NULL);
}

/* check if a word is a reserved word - used while scanning */
enum LexToken LexCheckReservedWord(const char *Word)
{
    int Count;
    
    for (Count = 0; Count < sizeof(ReservedWords) / sizeof(struct ReservedWord); Count++)
    {
        if (Word == ReservedWords[Count].SharedWord)
            return ReservedWords[Count].Token;
    }
    
    return TokenNone;
}

/* get a numeric literal - used while scanning */
enum LexToken LexGetNumber(struct LexState *Lexer, struct Value *Value)
{
    int Result = 0;
    int Base = 10;
    enum LexToken ResultToken;
#ifndef NO_FP
    double FPResult;
    double FPDiv;
#endif
    
    if (*Lexer->Pos == '0')
    { 
        /* a binary, octal or hex literal */
        LEXER_INC(Lexer);
        if (Lexer->Pos != Lexer->End)
        {
            if (*Lexer->Pos == 'x' || *Lexer->Pos == 'X')
                { Base = 16; LEXER_INC(Lexer); }
            else if (*Lexer->Pos == 'b' || *Lexer->Pos == 'B')
                { Base = 2; LEXER_INC(Lexer); }
            else if (*Lexer->Pos != '.')
                Base = 8;
        }
    }

    /* get the value */
    for (; Lexer->Pos != Lexer->End && IS_BASE_DIGIT(*Lexer->Pos, Base); LEXER_INC(Lexer))
        Result = Result * Base + GET_BASE_DIGIT(*Lexer->Pos);
    
    if (Result >= 0 && Result <= MAX_CHAR_VALUE)
    {
        Value->Typ = &CharType;
        Value->Val->Character = Result;
        ResultToken = TokenCharacterConstant;
    }
    else
    {
        Value->Typ = &IntType;
        Value->Val->Integer = Result;
        ResultToken = TokenIntegerConstant;
    }
    
    if (Lexer->Pos == Lexer->End)
        return ResultToken;
        
    if (*Lexer->Pos == 'l' || *Lexer->Pos == 'L')
    {
        LEXER_INC(Lexer);
        return ResultToken;
    }
        
#ifndef NO_FP
    if (Lexer->Pos == Lexer->End || *Lexer->Pos != '.')
        return ResultToken;

    Value->Typ = &FPType;
    LEXER_INC(Lexer);
    for (FPDiv = 1.0/Base, FPResult = (double)Result; Lexer->Pos != Lexer->End && IS_BASE_DIGIT(*Lexer->Pos, Base); LEXER_INC(Lexer), FPDiv /= (double)Base)
        FPResult += GET_BASE_DIGIT(*Lexer->Pos) * FPDiv;
    
    if (Lexer->Pos != Lexer->End && (*Lexer->Pos == 'e' || *Lexer->Pos == 'E'))
    {
        double ExponentMultiplier = 1.0;
        
        LEXER_INC(Lexer);
        if (Lexer->Pos != Lexer->End && *Lexer->Pos == '-')
        {
            ExponentMultiplier = -1.0;
            LEXER_INC(Lexer);
        }
        
        for (Result = 0; Lexer->Pos != Lexer->End && IS_BASE_DIGIT(*Lexer->Pos, Base); LEXER_INC(Lexer))
            Result = Result * (double)Base + GET_BASE_DIGIT(*Lexer->Pos);
            
        FPResult *= pow((double)Base, (double)Result * ExponentMultiplier);
    }
    
    Value->Val->FP = FPResult;
    
    return TokenFPConstant;
#else
    return ResultToken;
#endif
}

/* get a reserved word or identifier - used while scanning */
enum LexToken LexGetWord(struct LexState *Lexer, struct Value *Value)
{
    const char *StartPos = Lexer->Pos;
    enum LexToken Token;
    
    do {
        LEXER_INC(Lexer);
    } while (Lexer->Pos != Lexer->End && isCident((int)*Lexer->Pos));
    
    Value->Typ = NULL;
    Value->Val->Identifier = TableStrRegister2(StartPos, Lexer->Pos - StartPos);
    
    Token = LexCheckReservedWord(Value->Val->Identifier);
    switch (Token)
    {
        case TokenHashInclude: Lexer->Mode = LexModeHashInclude; break;
        case TokenHashDefine: Lexer->Mode = LexModeHashDefine; break;
        default: break;
    }
        
    if (Token != TokenNone)
        return Token;
    
    if (Lexer->Mode == LexModeHashDefineSpace)
        Lexer->Mode = LexModeHashDefineSpaceIdent;
    
    return TokenIdentifier;
}

/* unescape a character from an octal character constant */
unsigned char LexUnEscapeCharacterConstant(const char **From, const char *End, unsigned char FirstChar, int Base)
{
    unsigned char Total = GET_BASE_DIGIT(FirstChar);
    int CCount;
    for (CCount = 0; IS_BASE_DIGIT(**From, Base) && CCount < 2; CCount++, (*From)++)
        Total = Total * Base + GET_BASE_DIGIT(**From);
    
    return Total;
}

/* unescape a character from a string or character constant */
unsigned char LexUnEscapeCharacter(const char **From, const char *End)
{
    unsigned char ThisChar;
    
    while ( *From != End && **From == '\\' && 
            &(*From)[1] != End && (*From)[1] == '\n' )
        (*From) += 2;       /* skip escaped end of lines with LF line termination */
    
    while ( *From != End && **From == '\\' && 
            &(*From)[1] != End && &(*From)[2] != End && (*From)[1] == '\r' && (*From)[2] == '\n')
        (*From) += 3;       /* skip escaped end of lines with CR/LF line termination */
    
    if (*From == End)
        return '\\';
    
    if (**From == '\\')
    { 
        /* it's escaped */
        (*From)++;
        if (*From == End)
            return '\\';
        
        ThisChar = *(*From)++;
        switch (ThisChar)
        {
            case '\\': return '\\'; 
            case '\'': return '\'';
            case '"':  return '"';
            case 'a':  return '\a';
            case 'b':  return '\b';
            case 'f':  return '\f';
            case 'n':  return '\n';
            case 'r':  return '\r';
            case 't':  return '\t';
            case 'v':  return '\v';
            case '0': case '1': case '2': case '3': return LexUnEscapeCharacterConstant(From, End, ThisChar, 8);
            case 'x': return LexUnEscapeCharacterConstant(From, End, '0', 16);
            default:   return ThisChar;
        }
    }
    else
        return *(*From)++;
}

/* get a string constant - used while scanning */
enum LexToken LexGetStringConstant(struct LexState *Lexer, struct Value *Value, char EndChar)
{
    int Escape = FALSE;
    const char *StartPos = Lexer->Pos;
    const char *EndPos;
    char *EscBuf;
    char *EscBufPos;
    char *RegString;
    struct Value *ArrayValue;
    
    while (Lexer->Pos != Lexer->End && (*Lexer->Pos != EndChar || Escape))
    { 
        /* find the end */
        if (Escape)
        {
            if (*Lexer->Pos == '\r' && Lexer->Pos+1 != Lexer->End)
                Lexer->Pos++;
            
            if (*Lexer->Pos == '\n' && Lexer->Pos+1 != Lexer->End)
            {
                Lexer->Line++;
                Lexer->Pos++;
                Lexer->CharacterPos = 0;
                Lexer->EmitExtraNewlines++;
            }
            
            Escape = FALSE;
        }
        else if (*Lexer->Pos == '\\')
            Escape = TRUE;
            
        LEXER_INC(Lexer);
    }
    EndPos = Lexer->Pos;
    
    EscBuf = HeapAllocStack(EndPos - StartPos);
    if (EscBuf == NULL)
        LexFail(Lexer, "out of memory");
    
    for (EscBufPos = EscBuf, Lexer->Pos = StartPos; Lexer->Pos != EndPos;)
        *EscBufPos++ = LexUnEscapeCharacter(&Lexer->Pos, EndPos);
    
    /* try to find an existing copy of this string literal */
    RegString = TableStrRegister2(EscBuf, EscBufPos - EscBuf);
    HeapPopStack(EscBuf, EndPos - StartPos);
    ArrayValue = VariableStringLiteralGet(RegString);
    if (ArrayValue == NULL)
    {
        /* create and store this string literal */
        ArrayValue = VariableAllocValueAndData(NULL, 0, FALSE, NULL, TRUE);
        ArrayValue->Typ = CharArrayType;
        ArrayValue->Val = (union AnyValue *)RegString;
        VariableStringLiteralDefine(RegString, ArrayValue);
    }

    /* create the the pointer for this char* */
    Value->Typ = CharPtrType;
    Value->Val->Pointer = RegString;
    if (*Lexer->Pos == EndChar)
        LEXER_INC(Lexer);
    
    return TokenStringConstant;
}

/* get a character constant - used while scanning */
enum LexToken LexGetCharacterConstant(struct LexState *Lexer, struct Value *Value)
{
    Value->Typ = &CharType;
    Value->Val->Character = LexUnEscapeCharacter(&Lexer->Pos, Lexer->End);
    if (Lexer->Pos != Lexer->End && *Lexer->Pos != '\'')
        LexFail(Lexer, "expected \"'\"");
        
    LEXER_INC(Lexer);
    return TokenCharacterConstant;
}

/* skip a comment - used while scanning */
void LexSkipComment(struct LexState *Lexer, char NextChar, enum LexToken *ReturnToken)
{
    if (NextChar == '*')
    {   
        /* conventional C comment */
        while (Lexer->Pos != Lexer->End && (*(Lexer->Pos-1) != '*' || *Lexer->Pos != '/'))
        {
            if (*Lexer->Pos == '\n')
                Lexer->EmitExtraNewlines++;

            LEXER_INC(Lexer);
        }
        
        if (Lexer->Pos != Lexer->End)
            LEXER_INC(Lexer);
        
        Lexer->Mode = LexModeNormal;
    }
    else
    {   
        /* C++ style comment */
        while (Lexer->Pos != Lexer->End && *Lexer->Pos != '\n')
            LEXER_INC(Lexer);
    }
}

/* get a single token from the source - used while scanning */
enum LexToken LexScanGetToken(struct LexState *Lexer, struct Value **Value)
{
    char ThisChar;
    char NextChar;
    enum LexToken GotToken = TokenNone;
    
    /* handle cases line multi-line comments or string constants which mess up the line count */
    if (Lexer->EmitExtraNewlines > 0)
    {
        Lexer->EmitExtraNewlines--;
        return TokenEndOfLine;
    }
    
    /* scan for a token */
    do
    {
        *Value = &LexValue;
        while (Lexer->Pos != Lexer->End && isspace((int)*Lexer->Pos))
        {
            if (*Lexer->Pos == '\n')
            {
                Lexer->Line++;
                Lexer->Pos++;
                Lexer->Mode = LexModeNormal;
                Lexer->CharacterPos = 0;
                return TokenEndOfLine;
            }
            else if (Lexer->Mode == LexModeHashDefine || Lexer->Mode == LexModeHashDefineSpace)
                Lexer->Mode = LexModeHashDefineSpace;
            
            else if (Lexer->Mode == LexModeHashDefineSpaceIdent)
                Lexer->Mode = LexModeNormal;
    
            LEXER_INC(Lexer);
        }
        
        if (Lexer->Pos == Lexer->End || *Lexer->Pos == '\0')
            return TokenEOF;
        
        ThisChar = *Lexer->Pos;
        if (isCidstart((int)ThisChar))
            return LexGetWord(Lexer, *Value);
        
        if (isdigit((int)ThisChar))
            return LexGetNumber(Lexer, *Value);
        
        NextChar = (Lexer->Pos+1 != Lexer->End) ? *(Lexer->Pos+1) : 0;
        LEXER_INC(Lexer);
        switch (ThisChar)
        {
            case '"': GotToken = LexGetStringConstant(Lexer, *Value, '"'); break;
            case '\'': GotToken = LexGetCharacterConstant(Lexer, *Value); break;
            case '(': if (Lexer->Mode == LexModeHashDefineSpaceIdent) GotToken = TokenOpenMacroBracket; else GotToken = TokenOpenBracket; Lexer->Mode = LexModeNormal; break;
            case ')': GotToken = TokenCloseBracket; break;
            case '=': NEXTIS('=', TokenEqual, TokenAssign); break;
            case '+': NEXTIS3('=', TokenAddAssign, '+', TokenIncrement, TokenPlus); break;
            case '-': NEXTIS4('=', TokenSubtractAssign, '>', TokenArrow, '-', TokenDecrement, TokenMinus); break;
            case '*': NEXTIS('=', TokenMultiplyAssign, TokenAsterisk); break;
            case '/': if (NextChar == '/' || NextChar == '*') { LEXER_INC(Lexer); LexSkipComment(Lexer, NextChar, &GotToken); } else NEXTIS('=', TokenDivideAssign, TokenSlash); break;
            case '%': NEXTIS('=', TokenModulusAssign, TokenModulus); break;
            case '<': if (Lexer->Mode == LexModeHashInclude) GotToken = LexGetStringConstant(Lexer, *Value, '>'); else { NEXTIS3PLUS('=', TokenLessEqual, '<', TokenShiftLeft, '=', TokenShiftLeftAssign, TokenLessThan); } break; 
            case '>': NEXTIS3PLUS('=', TokenGreaterEqual, '>', TokenShiftRight, '=', TokenShiftRightAssign, TokenGreaterThan); break;
            case ';': GotToken = TokenSemicolon; break;
            case '&': NEXTIS3('=', TokenArithmeticAndAssign, '&', TokenLogicalAnd, TokenAmpersand); break;
            case '|': NEXTIS3('=', TokenArithmeticOrAssign, '|', TokenLogicalOr, TokenArithmeticOr); break;
            case '{': GotToken = TokenLeftBrace; break;
            case '}': GotToken = TokenRightBrace; break;
            case '[': GotToken = TokenLeftSquareBracket; break;
            case ']': GotToken = TokenRightSquareBracket; break;
            case '!': NEXTIS('=', TokenNotEqual, TokenUnaryNot); break;
            case '^': NEXTIS('=', TokenArithmeticExorAssign, TokenArithmeticExor); break;
            case '~': GotToken = TokenUnaryExor; break;
            case ',': GotToken = TokenComma; break;
            case '.': NEXTISEXACTLY3('.', '.', TokenEllipsis, TokenDot); break;
            case '?': GotToken = TokenQuestionMark; break;
            case ':': GotToken = TokenColon; break;
            default:  LexFail(Lexer, "illegal character '%c'", ThisChar); break;
        }
    } while (GotToken == TokenNone);
    
    return GotToken;
}

/* what size value goes with each token */
int LexTokenSize(enum LexToken Token)
{
    switch (Token)
    {
        case TokenIdentifier: case TokenStringConstant: return sizeof(char *);
        case TokenIntegerConstant: return sizeof(int);
        case TokenCharacterConstant: return sizeof(unsigned char);
        case TokenFPConstant: return sizeof(double);
        default: return 0;
    }
}

/* produce tokens from the lexer and return a heap buffer with the result - used for scanning */
void *LexTokenise(struct LexState *Lexer, int *TokenLen)
{
    enum LexToken Token;
    void *HeapMem;
    struct Value *GotValue;
    int MemUsed = 0;
    int ValueSize;
    int ReserveSpace = (Lexer->End - Lexer->Pos) * 4 + 16; 
    void *TokenSpace = HeapAllocStack(ReserveSpace);
    char *TokenPos = (char *)TokenSpace;
    int LastCharacterPos = 0;

    if (TokenSpace == NULL)
        LexFail(Lexer, "out of memory");
    
    do
    { 
        /* store the token at the end of the stack area */
        Token = LexScanGetToken(Lexer, &GotValue);
        
#ifdef DEBUG_LEXER
        printf("Token: %02x\n", Token);
#endif
        *(unsigned char *)TokenPos = Token;
        TokenPos++;
        MemUsed++;

        *(unsigned char *)TokenPos = (unsigned char)LastCharacterPos;
        TokenPos++;
        MemUsed++;

        ValueSize = LexTokenSize(Token);
        if (ValueSize > 0)
        { 
            /* store a value as well */
            memcpy((void *)TokenPos, (void *)GotValue->Val, ValueSize);
            TokenPos += ValueSize;
            MemUsed += ValueSize;
        }
    
        LastCharacterPos = Lexer->CharacterPos;
                    
    } while (Token != TokenEOF);
    
    HeapMem = HeapAllocMem(MemUsed);
    if (HeapMem == NULL)
        LexFail(Lexer, "out of memory");
        
    assert(ReserveSpace >= MemUsed);
    memcpy(HeapMem, TokenSpace, MemUsed);
    HeapPopStack(TokenSpace, ReserveSpace);
#ifdef DEBUG_LEXER
    {
        int Count;
        printf("Tokens: ");
        for (Count = 0; Count < MemUsed; Count++)
            printf("%02x ", *((unsigned char *)HeapMem+Count));
        printf("\n");
    }
#endif
    if (TokenLen)
        *TokenLen = MemUsed;
    
    return HeapMem;
}

/* lexically analyse some source text */
void *LexAnalyse(const char *FileName, const char *Source, int SourceLen, int *TokenLen)
{
    struct LexState Lexer;
    
    Lexer.Pos = Source;
    Lexer.End = Source + SourceLen;
    Lexer.Line = 1;
    Lexer.FileName = FileName;
    Lexer.Mode = LexModeNormal;
    Lexer.EmitExtraNewlines = 0;
    Lexer.CharacterPos = 1;
    Lexer.SourceText = Source;
    
    return LexTokenise(&Lexer, TokenLen);
}

/* prepare to parse a pre-tokenised buffer */
void LexInitParser(struct ParseState *Parser, const char *SourceText, void *TokenSource, const char *FileName, int RunIt)
{
    Parser->Pos = TokenSource;
    Parser->Line = 1;
    Parser->FileName = FileName;
    Parser->Mode = RunIt ? RunModeRun : RunModeSkip;
    Parser->SearchLabel = 0;
    Parser->HashIfLevel = 0;
    Parser->HashIfEvaluateToLevel = 0;
    Parser->CharacterPos = 0;
    Parser->SourceText = SourceText;
}

/* get the next token, without pre-processing */
enum LexToken LexGetRawToken(struct ParseState *Parser, struct Value **Value, int IncPos)
{
    enum LexToken Token = TokenNone;
    int ValueSize;
    char *Prompt = NULL;
    
    do
    { 
        /* get the next token */
        if (Parser->Pos == NULL && InteractiveHead != NULL)
            Parser->Pos = InteractiveHead->Tokens;
        
        if (Parser->FileName != StrEmpty || InteractiveHead != NULL)
        { 
            /* skip leading newlines */
            while ((Token = (enum LexToken)*(unsigned char *)Parser->Pos) == TokenEndOfLine)
            {
                Parser->Line++;
                Parser->Pos += TOKEN_DATA_OFFSET;
            }
        }
    
        if (Parser->FileName == StrEmpty && (InteractiveHead == NULL || Token == TokenEOF))
        { 
            /* we're at the end of an interactive input token list */
            char LineBuffer[LINEBUFFER_MAX];
            void *LineTokens;
            int LineBytes;
            struct TokenLine *LineNode;
            
            if (InteractiveHead == NULL || (unsigned char *)Parser->Pos == &InteractiveTail->Tokens[InteractiveTail->NumBytes-TOKEN_DATA_OFFSET])
            { 
                /* get interactive input */
                if (LexUseStatementPrompt)
                {
                    Prompt = INTERACTIVE_PROMPT_STATEMENT;
                    LexUseStatementPrompt = FALSE;
                }
                else
                    Prompt = INTERACTIVE_PROMPT_LINE;
                    
                if (PlatformGetLine(&LineBuffer[0], LINEBUFFER_MAX, Prompt) == NULL)
                    return TokenEOF;

                /* put the new line at the end of the linked list of interactive lines */        
                LineTokens = LexAnalyse(StrEmpty, &LineBuffer[0], strlen(LineBuffer), &LineBytes);
                LineNode = VariableAlloc(Parser, sizeof(struct TokenLine), TRUE);
                LineNode->Tokens = LineTokens;
                LineNode->NumBytes = LineBytes;
                if (InteractiveHead == NULL)
                { 
                    /* start a new list */
                    InteractiveHead = LineNode;
                    Parser->Line = 1;
                    Parser->CharacterPos = 0;
                }
                else
                    InteractiveTail->Next = LineNode;

                InteractiveTail = LineNode;
                InteractiveCurrentLine = LineNode;
                Parser->Pos = LineTokens;
            }
            else
            { 
                /* go to the next token line */
                if (Parser->Pos != &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET])
                { 
                    /* scan for the line */
                    for (InteractiveCurrentLine = InteractiveHead; Parser->Pos != &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET]; InteractiveCurrentLine = InteractiveCurrentLine->Next)
                    { assert(InteractiveCurrentLine->Next != NULL); }
                }

                assert(InteractiveCurrentLine != NULL);
                InteractiveCurrentLine = InteractiveCurrentLine->Next;
                assert(InteractiveCurrentLine != NULL);
                Parser->Pos = InteractiveCurrentLine->Tokens;
            }

            Token = (enum LexToken)*(unsigned char *)Parser->Pos;
        }
    } while ((Parser->FileName == StrEmpty && Token == TokenEOF) || Token == TokenEndOfLine);

    Parser->CharacterPos = *((unsigned char *)Parser->Pos + 1);
    ValueSize = LexTokenSize(Token);
    if (ValueSize > 0)
    { 
        /* this token requires a value - unpack it */
        if (Value != NULL)
        { 
            switch (Token)
            {
                case TokenStringConstant:       LexValue.Typ = CharPtrType; break;
                case TokenIdentifier:           LexValue.Typ = NULL; break;
                case TokenIntegerConstant:      LexValue.Typ = &IntType; break;
                case TokenCharacterConstant:    LexValue.Typ = &CharType; break;
#ifndef NO_FP
                case TokenFPConstant:           LexValue.Typ = &FPType; break;
#endif
                default: break;
            }
            
            memcpy((void *)LexValue.Val, (void *)((char *)Parser->Pos + TOKEN_DATA_OFFSET), ValueSize);
            LexValue.ValOnHeap = FALSE;
            LexValue.ValOnStack = FALSE;
            LexValue.IsLValue = FALSE;
            LexValue.LValueFrom = NULL;
            *Value = &LexValue;
        }
        
        if (IncPos)
            Parser->Pos += ValueSize + TOKEN_DATA_OFFSET;
    }
    else
    {
        if (IncPos && Token != TokenEOF)
            Parser->Pos += TOKEN_DATA_OFFSET;
    }
    
#ifdef DEBUG_LEXER
    printf("Got token=%02x inc=%d pos=%d\n", Token, IncPos, Parser->CharacterPos);
#endif
    assert(Token >= TokenNone && Token <= TokenEndOfFunction);
    return Token;
}

/* correct the token position depending if we already incremented the position */
void LexHashIncPos(struct ParseState *Parser, int IncPos)
{
    if (!IncPos)
        LexGetRawToken(Parser, NULL, TRUE);
}

/* handle a #ifdef directive */
void LexHashIfdef(struct ParseState *Parser, int IfNot)
{
    /* get symbol to check */
    struct Value *IdentValue;
    struct Value *SavedValue;
    int IsDefined;
    enum LexToken Token = LexGetRawToken(Parser, &IdentValue, TRUE);
    
    if (Token != TokenIdentifier)
        ProgramFail(Parser, "identifier expected");
    
    /* is the identifier defined? */
    IsDefined = TableGet(&GlobalTable, IdentValue->Val->Identifier, &SavedValue, NULL, NULL, NULL);
    if (Parser->HashIfEvaluateToLevel == Parser->HashIfLevel && ( (IsDefined && !IfNot) || (!IsDefined && IfNot)) )
    {
        /* #if is active, evaluate to this new level */
        Parser->HashIfEvaluateToLevel++;
    }
    
    Parser->HashIfLevel++;
}

/* handle a #if directive */
void LexHashIf(struct ParseState *Parser)
{
    /* get symbol to check */
    struct Value *IdentValue;
    struct Value *SavedValue;
    struct ParseState MacroParser;
    enum LexToken Token = LexGetRawToken(Parser, &IdentValue, TRUE);

    if (Token == TokenIdentifier)
    {
        /* look up a value from a macro definition */
        if (!TableGet(&GlobalTable, IdentValue->Val->Identifier, &SavedValue, NULL, NULL, NULL))
            ProgramFail(Parser, "'%s' is undefined", IdentValue->Val->Identifier);
        
        if (SavedValue->Typ->Base != TypeMacro)
            ProgramFail(Parser, "value expected");
        
        ParserCopy(&MacroParser, &SavedValue->Val->MacroDef.Body);
        Token = LexGetRawToken(&MacroParser, &IdentValue, TRUE);
    }
    
    if (Token != TokenCharacterConstant)
        ProgramFail(Parser, "value expected");
    
    /* is the identifier defined? */
    if (Parser->HashIfEvaluateToLevel == Parser->HashIfLevel && IdentValue->Val->Character)
    {
        /* #if is active, evaluate to this new level */
        Parser->HashIfEvaluateToLevel++;
    }
    
    Parser->HashIfLevel++;
}

/* handle a #else directive */
void LexHashElse(struct ParseState *Parser)
{
    if (Parser->HashIfEvaluateToLevel == Parser->HashIfLevel - 1)
        Parser->HashIfEvaluateToLevel++;     /* #if was not active, make this next section active */
        
    else if (Parser->HashIfEvaluateToLevel == Parser->HashIfLevel)
    {
        /* #if was active, now go inactive */
        if (Parser->HashIfLevel == 0)
            ProgramFail(Parser, "#else without #if");
            
        Parser->HashIfEvaluateToLevel--;
    }
}

/* handle a #endif directive */
void LexHashEndif(struct ParseState *Parser)
{
    if (Parser->HashIfLevel == 0)
        ProgramFail(Parser, "#endif without #if");

    Parser->HashIfLevel--;
    if (Parser->HashIfEvaluateToLevel > Parser->HashIfLevel)
        Parser->HashIfEvaluateToLevel = Parser->HashIfLevel;
}

/* get the next token given a parser state, pre-processing as we go */
enum LexToken LexGetToken(struct ParseState *Parser, struct Value **Value, int IncPos)
{
    enum LexToken Token;
    int TryNextToken;
    
    /* implements the pre-processor #if commands */
    do
    {
        int WasPreProcToken = TRUE;

        Token = LexGetRawToken(Parser, Value, IncPos);
        switch (Token)
        {
            case TokenHashIfdef:    LexHashIncPos(Parser, IncPos); LexHashIfdef(Parser, FALSE); break;
            case TokenHashIfndef:   LexHashIncPos(Parser, IncPos); LexHashIfdef(Parser, TRUE); break;
            case TokenHashIf:       LexHashIncPos(Parser, IncPos); LexHashIf(Parser); break;
            case TokenHashElse:     LexHashIncPos(Parser, IncPos); LexHashElse(Parser); break;
            case TokenHashEndif:    LexHashIncPos(Parser, IncPos); LexHashEndif(Parser); break;
            default:                WasPreProcToken = FALSE; break;
        }

        /* if we're going to reject this token, increment the token pointer to the next one */
        TryNextToken = (Parser->HashIfEvaluateToLevel < Parser->HashIfLevel && Token != TokenEOF) || WasPreProcToken;
        if (!IncPos && TryNextToken)
            LexGetRawToken(Parser, NULL, TRUE);
            
    } while (TryNextToken);
    
    return Token;
}

/* take a quick peek at the next token, skipping any pre-processing */
enum LexToken LexRawPeekToken(struct ParseState *Parser)
{
    return (enum LexToken)*(unsigned char *)Parser->Pos;
}

/* find the end of the line */
void LexToEndOfLine(struct ParseState *Parser)
{
    while (TRUE)
    {
        enum LexToken Token = (enum LexToken)*(unsigned char *)Parser->Pos;
        if (Token == TokenEndOfLine || Token == TokenEOF)
            return;
        else
            LexGetRawToken(Parser, NULL, TRUE);
    }
}

/* copy the tokens from StartParser to EndParser into new memory, removing TokenEOFs and terminate with a TokenEndOfFunction */
void *LexCopyTokens(struct ParseState *StartParser, struct ParseState *EndParser)
{
    int MemSize = 0;
    int CopySize;
    unsigned char *Pos = (unsigned char *)StartParser->Pos;
    unsigned char *NewTokens;
    unsigned char *NewTokenPos;
    struct TokenLine *ILine;
    
    if (InteractiveHead == NULL)
    { 
        /* non-interactive mode - copy the tokens */
        MemSize = EndParser->Pos - StartParser->Pos;
        NewTokens = VariableAlloc(StartParser, MemSize + TOKEN_DATA_OFFSET, TRUE);
        memcpy(NewTokens, (void *)StartParser->Pos, MemSize);
    }
    else
    { 
        /* we're in interactive mode - add up line by line */
        for (InteractiveCurrentLine = InteractiveHead; InteractiveCurrentLine != NULL && (Pos < &InteractiveCurrentLine->Tokens[0] || Pos >= &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes]); InteractiveCurrentLine = InteractiveCurrentLine->Next)
        {} /* find the line we just counted */
        
        if (EndParser->Pos >= StartParser->Pos && EndParser->Pos < &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes])
        { 
            /* all on a single line */
            MemSize = EndParser->Pos - StartParser->Pos;
            NewTokens = VariableAlloc(StartParser, MemSize + TOKEN_DATA_OFFSET, TRUE);
            memcpy(NewTokens, (void *)StartParser->Pos, MemSize);
        }
        else
        { 
            /* it's spread across multiple lines */
            MemSize = &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET] - Pos;

            for (ILine = InteractiveCurrentLine->Next; ILine != NULL && (EndParser->Pos < &ILine->Tokens[0] || EndParser->Pos >= &ILine->Tokens[ILine->NumBytes]); ILine = ILine->Next)
                MemSize += ILine->NumBytes - TOKEN_DATA_OFFSET;
            
            assert(ILine != NULL);
            MemSize += EndParser->Pos - &ILine->Tokens[0];
            NewTokens = VariableAlloc(StartParser, MemSize + TOKEN_DATA_OFFSET, TRUE);
            
            CopySize = &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET] - Pos;
            memcpy(NewTokens, Pos, CopySize);
            NewTokenPos = NewTokens + CopySize;
            for (ILine = InteractiveCurrentLine->Next; ILine != NULL && (EndParser->Pos < &ILine->Tokens[0] || EndParser->Pos >= &ILine->Tokens[ILine->NumBytes]); ILine = ILine->Next)
            {
                memcpy(NewTokenPos, &ILine->Tokens[0], ILine->NumBytes - TOKEN_DATA_OFFSET);
                NewTokenPos += ILine->NumBytes-TOKEN_DATA_OFFSET;
            }
            assert(ILine != NULL);
            memcpy(NewTokenPos, &ILine->Tokens[0], EndParser->Pos - &ILine->Tokens[0]);
        }
    }
    
    NewTokens[MemSize] = (unsigned char)TokenEndOfFunction;
        
    return NewTokens;
}

/* indicate that we've completed up to this point in the interactive input and free expired tokens */
void LexInteractiveClear(struct ParseState *Parser)
{
    while (InteractiveHead != NULL)
    {
        struct TokenLine *NextLine = InteractiveHead->Next;
        
        HeapFreeMem(InteractiveHead->Tokens);
        HeapFreeMem(InteractiveHead);
        InteractiveHead = NextLine;
    }

    if (Parser != NULL)
        Parser->Pos = NULL;
    InteractiveTail = NULL;
}

/* indicate that we've completed up to this point in the interactive input and free expired tokens */
void LexInteractiveCompleted(struct ParseState *Parser)
{
    while (InteractiveHead != NULL && !(Parser->Pos >= &InteractiveHead->Tokens[0] && Parser->Pos < &InteractiveHead->Tokens[InteractiveHead->NumBytes]))
    { 
        /* this token line is no longer needed - free it */
        struct TokenLine *NextLine = InteractiveHead->Next;
        
        HeapFreeMem(InteractiveHead->Tokens);
        HeapFreeMem(InteractiveHead);
        InteractiveHead = NextLine;
        
        if (InteractiveHead == NULL)
        { 
            /* we've emptied the list */
            Parser->Pos = NULL;
            InteractiveTail = NULL;
        }
    }
}

/* the next time we prompt, make it the full statement prompt */
void LexInteractiveStatementPrompt()
{
    LexUseStatementPrompt = TRUE;
}
