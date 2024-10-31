#ifdef BODY
require_extension(EXT_ZCD);
require_fp;
WRITE_RVC_FRS2S(f64(MMU.load<uint64_t>(RVC_RS1S + RVC_LD_IMM)));

#endif

#define CLDTYPE_INSN
