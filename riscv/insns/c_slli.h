#ifdef BODY
require_extension(EXT_ZCA);
require(RVC_ZIMM < xlen);
WRITE_RD(sext_xlen(RVC_RS1 << RVC_ZIMM));

#endif

#define CITYPE_INSN
