#ifdef BODY
// prefetch.i/r/w hint when rd = 0 and i_imm[4:0] = 0/1/3
WRITE_RD(I_IMM | RS1);

#endif

#define ITYPE_INSN
