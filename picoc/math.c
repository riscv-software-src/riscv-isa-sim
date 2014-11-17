/* stdio.h library for large systems - small embedded systems use clibrary.c instead */
#include "interpreter.h"

#ifndef BUILTIN_MINI_STDLIB
#ifndef NO_FP

static double M_EValue =        2.7182818284590452354;   /* e */
static double M_LOG2EValue =    1.4426950408889634074;   /* log_2 e */
static double M_LOG10EValue =   0.43429448190325182765;  /* log_10 e */
static double M_LN2Value =      0.69314718055994530942;  /* log_e 2 */
static double M_LN10Value =     2.30258509299404568402;  /* log_e 10 */
static double M_PIValue =       3.14159265358979323846;  /* pi */
static double M_PI_2Value =     1.57079632679489661923;  /* pi/2 */
static double M_PI_4Value =     0.78539816339744830962;  /* pi/4 */
static double M_1_PIValue =     0.31830988618379067154;  /* 1/pi */
static double M_2_PIValue =     0.63661977236758134308;  /* 2/pi */
static double M_2_SQRTPIValue = 1.12837916709551257390;  /* 2/sqrt(pi) */
static double M_SQRT2Value =    1.41421356237309504880;  /* sqrt(2) */
static double M_SQRT1_2Value =  0.70710678118654752440;  /* 1/sqrt(2) */


void MathSin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = sin(Param[0]->Val->FP);
}

void MathCos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = cos(Param[0]->Val->FP);
}

void MathTan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = tan(Param[0]->Val->FP);
}

void MathAsin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = asin(Param[0]->Val->FP);
}

void MathAcos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = acos(Param[0]->Val->FP);
}

void MathAtan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = atan(Param[0]->Val->FP);
}

void MathAtan2(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = atan2(Param[0]->Val->FP, Param[1]->Val->FP);
}

void MathSinh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = sinh(Param[0]->Val->FP);
}

void MathCosh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = cosh(Param[0]->Val->FP);
}

void MathTanh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = tanh(Param[0]->Val->FP);
}

void MathExp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = exp(Param[0]->Val->FP);
}

void MathFabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = fabs(Param[0]->Val->FP);
}

void MathFmod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = fmod(Param[0]->Val->FP, Param[1]->Val->FP);
}

void MathFrexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = frexp(Param[0]->Val->FP, Param[1]->Val->Pointer);
}

void MathLdexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = ldexp(Param[0]->Val->FP, Param[1]->Val->Integer);
}

void MathLog(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = log(Param[0]->Val->FP);
}

void MathLog10(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = log10(Param[0]->Val->FP);
}

void MathModf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = modf(Param[0]->Val->FP, Param[0]->Val->Pointer);
}

void MathPow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = pow(Param[0]->Val->FP, Param[1]->Val->FP);
}

void MathSqrt(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = sqrt(Param[0]->Val->FP);
}

void MathRound(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    /* this awkward definition of "round()" due to it being inconsistently
     * declared in math.h */
    ReturnValue->Val->FP = ceil(Param[0]->Val->FP - 0.5);
}

void MathCeil(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = ceil(Param[0]->Val->FP);
}

void MathFloor(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = floor(Param[0]->Val->FP);
}

/* all math.h functions */
struct LibraryFunction MathFunctions[] =
{
    { MathAcos,         "float acos(float);" },
    { MathAsin,         "float asin(float);" },
    { MathAtan,         "float atan(float);" },
    { MathAtan2,        "float atan2(float, float);" },
    { MathCeil,         "float ceil(float);" },
    { MathCos,          "float cos(float);" },
    { MathCosh,         "float cosh(float);" },
    { MathExp,          "float exp(float);" },
    { MathFabs,         "float fabs(float);" },
    { MathFloor,        "float floor(float);" },
    { MathFmod,         "float fmod(float, float);" },
    { MathFrexp,        "float frexp(float, int *);" },
    { MathLdexp,        "float ldexp(float, int);" },
    { MathLog,          "float log(float);" },
    { MathLog10,        "float log10(float);" },
    { MathModf,         "float modf(float, float *);" },
    { MathPow,          "float pow(float,float);" },
    { MathRound,        "float round(float);" },
    { MathSin,          "float sin(float);" },
    { MathSinh,         "float sinh(float);" },
    { MathSqrt,         "float sqrt(float);" },
    { MathTan,          "float tan(float);" },
    { MathTanh,         "float tanh(float);" },
    { NULL,             NULL }
};

/* creates various system-dependent definitions */
void MathSetupFunc(void)
{
    VariableDefinePlatformVar(NULL, "M_E", &FPType, (union AnyValue *)&M_EValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_LOG2E", &FPType, (union AnyValue *)&M_LOG2EValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_LOG10E", &FPType, (union AnyValue *)&M_LOG10EValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_LN2", &FPType, (union AnyValue *)&M_LN2Value, FALSE);
    VariableDefinePlatformVar(NULL, "M_LN10", &FPType, (union AnyValue *)&M_LN10Value, FALSE);
    VariableDefinePlatformVar(NULL, "M_PI", &FPType, (union AnyValue *)&M_PIValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_PI_2", &FPType, (union AnyValue *)&M_PI_2Value, FALSE);
    VariableDefinePlatformVar(NULL, "M_PI_4", &FPType, (union AnyValue *)&M_PI_4Value, FALSE);
    VariableDefinePlatformVar(NULL, "M_1_PI", &FPType, (union AnyValue *)&M_1_PIValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_2_PI", &FPType, (union AnyValue *)&M_2_PIValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_2_SQRTPI", &FPType, (union AnyValue *)&M_2_SQRTPIValue, FALSE);
    VariableDefinePlatformVar(NULL, "M_SQRT2", &FPType, (union AnyValue *)&M_SQRT2Value, FALSE);
    VariableDefinePlatformVar(NULL, "M_SQRT1_2", &FPType, (union AnyValue *)&M_SQRT1_2Value, FALSE);
}

#endif /* !NO_FP */
#endif /* !BUILTIN_MINI_STDLIB */
