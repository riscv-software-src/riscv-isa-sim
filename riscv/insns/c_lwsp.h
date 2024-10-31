#ifdef BODY
require_extension(EXT_ZCA);
require(insn.cilwtype.rvc_rd != 0);
WRITE_RD(MMU.load<int32_t>(RVC_SP + RVC_LWSP_IMM));

#endif

#define CILWTYPE_INSN
