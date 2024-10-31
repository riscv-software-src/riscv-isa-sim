#ifdef BODY
require(SHAMT < xlen);
WRITE_RD(sext_xlen(sext_xlen(RS1) >> SHAMT));

#endif

#define ITYPE_INSN
