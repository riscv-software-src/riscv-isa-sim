#include "picoc.h"
#include "interpreter.h"

int last_spike_result = 0;

// Defined by spike
int picoc_get_pc(int p);
int picoc_get_reg(int p, int r);
int picoc_get_mem(char* addr);

void GetReg(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  ReturnValue->Val->Integer = picoc_get_reg(Param[0]->Val->Integer, Param[1]->Val->Integer);
}

void GetPc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  ReturnValue->Val->Integer = picoc_get_pc(Param[0]->Val->Integer);
}

void GetMem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  ReturnValue->Val->Integer = picoc_get_mem((char*)Param[0]->Val->Pointer);
}

struct LibraryFunction SpikeFunctions[] =
{
    { GetReg,           "int reg(int,int);" },
    { GetPc,           "int pc(int);" },
    { GetMem,           "int mem(char*);" },
    { NULL,             NULL }
};

void SpikeLibrarySetupFunc(void) {
  int i = 0;

  VariableDefinePlatformVar(NULL, "result", &IntType, (union AnyValue*)&last_spike_result, TRUE);

  #define NUM_DEFINES 7
  char *defs[NUM_DEFINES] = {
    "FILE* fw;", // Required declarations

    "#define px(x) printf(\"0x%x\n\", x);", // Print in hexadecimal
    "#define pd(x) printf(\"%d\n\", x);", // Print decimal

    "void wf(char *x) { fw = fopen(x, \"w\"); }", // Open file for writing
    "#define fx(x) fprintf(fw, \"0x%x\n\", x);", // Print in hexadecimal to file
    "#define fd(x) fprintf(fw, \"%d\n\", x);", // Print decimal to file
    "#define cwf() fclose(fw);", // Close write file
  };

  for (i = 0; i < NUM_DEFINES; i++) {
    PicocParse("spike lib", defs[i], strlen(defs[i]), TRUE, TRUE, FALSE);
  }
}
