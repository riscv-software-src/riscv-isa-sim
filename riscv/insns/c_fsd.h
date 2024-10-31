#ifdef BODY
require_extension(EXT_ZCD);
require_fp;
MMU.store<uint64_t>(RVC_RS1S + RVC_LD_IMM, RVC_FRS2S.v[0]);

#endif

#define CSDTYPE_INSN
