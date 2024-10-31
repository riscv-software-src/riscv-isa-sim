#ifdef BODY
require_extension(EXT_ZCD);
require_fp;
MMU.store<uint64_t>(RVC_SP + RVC_SDSP_IMM, RVC_FRS2.v[0]);

#endif

#define CSSSDTYPE_INSN
