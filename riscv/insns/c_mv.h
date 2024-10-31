#ifdef BODY
require_extension(EXT_ZCA);
require(insn.crtype.rvc_rs2 != 0);
WRITE_RD(RVC_RS2);

#endif

#define CRTYPE_INSN
