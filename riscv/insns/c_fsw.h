#ifdef BODY
require_extension(EXT_ZCF);
require_fp;
MMU.store<uint32_t>(RVC_RS1S + RVC_LW_IMM, RVC_FRS2S.v[0]);

#endif

#define CSWTYPE_INSN
