#ifdef BODY
require_extension(EXT_ZCF);
require_fp;
WRITE_FRD(f32(MMU.load<uint32_t>(RVC_SP + RVC_LWSP_IMM)));

#endif

#define CILWTYPE_INSN
