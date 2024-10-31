#ifdef BODY
require_extension(EXT_ZCA);
require(RVC_ZIMM < xlen);
WRITE_RVC_RS1S(sext_xlen(zext_xlen(RVC_RS1S) >> RVC_ZIMM));

#endif

#define CITYPE_INSN
