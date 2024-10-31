#ifdef BODY
require_extension(EXT_ZCF);
require_fp;
WRITE_RVC_FRS2S(f32(MMU.load<uint32_t>(RVC_RS1S + RVC_LW_IMM)));

#endif

#define CLWTYPE_INSN
