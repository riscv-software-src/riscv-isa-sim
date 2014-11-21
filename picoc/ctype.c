/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <ctype.h>
#include "interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

void StdIsalnum(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isalnum(Param[0]->Val->Integer);
}

void StdIsalpha(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isalpha(Param[0]->Val->Integer);
}

void StdIsblank(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int ch = Param[0]->Val->Integer;
    ReturnValue->Val->Integer = (ch == ' ') | (ch == '\t');
}

void StdIscntrl(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = iscntrl(Param[0]->Val->Integer);
}

void StdIsdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isdigit(Param[0]->Val->Integer);
}

void StdIsgraph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isgraph(Param[0]->Val->Integer);
}

void StdIslower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = islower(Param[0]->Val->Integer);
}

void StdIsprint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isprint(Param[0]->Val->Integer);
}

void StdIspunct(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = ispunct(Param[0]->Val->Integer);
}

void StdIsspace(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isspace(Param[0]->Val->Integer);
}

void StdIsupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isupper(Param[0]->Val->Integer);
}

void StdIsxdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isxdigit(Param[0]->Val->Integer);
}

void StdTolower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = tolower(Param[0]->Val->Integer);
}

void StdToupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = toupper(Param[0]->Val->Integer);
}

void StdIsascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isascii(Param[0]->Val->Integer);
}

void StdToascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = toascii(Param[0]->Val->Integer);
}

/* all string.h functions */
struct LibraryFunction StdCtypeFunctions[] =
{
    { StdIsalnum,      "int isalnum(int);" },
    { StdIsalpha,      "int isalpha(int);" },
    { StdIsblank,      "int isblank(int);" },
    { StdIscntrl,      "int iscntrl(int);" },
    { StdIsdigit,      "int isdigit(int);" },
    { StdIsgraph,      "int isgraph(int);" },
    { StdIslower,      "int islower(int);" },
    { StdIsprint,      "int isprint(int);" },
    { StdIspunct,      "int ispunct(int);" },
    { StdIsspace,      "int isspace(int);" },
    { StdIsupper,      "int isupper(int);" },
    { StdIsxdigit,     "int isxdigit(int);" },
    { StdTolower,      "int tolower(int);" },
    { StdToupper,      "int toupper(int);" },
    { StdIsascii,      "int isascii(int);" },
    { StdToascii,      "int toascii(int);" },
    { NULL,             NULL }
};

#endif /* !BUILTIN_MINI_STDLIB */
