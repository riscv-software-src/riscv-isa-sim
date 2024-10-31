#ifdef BODY
require_rv64;
WRITE_RD(sext32(I_IMM + RS1));

#endif

#define ITYPE_INSN
