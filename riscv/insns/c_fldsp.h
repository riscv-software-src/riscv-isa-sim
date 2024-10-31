#ifdef BODY
require_extension(EXT_ZCD);
require_fp;
WRITE_FRD(f64(MMU.load<uint64_t>(RVC_SP + RVC_LDSP_IMM)));

#endif

#define CSSLDTYPE_INSN
