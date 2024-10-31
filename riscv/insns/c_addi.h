#ifdef BODY
require_extension(EXT_ZCA);
WRITE_RD(sext_xlen(RVC_RS1 + RVC_IMM));

#endif

#define CITYPE_INSN
