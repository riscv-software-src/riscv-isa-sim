#ifdef BODY
require_extension(EXT_ZCA);
WRITE_RVC_RS2S(MMU.load<int32_t>(RVC_RS1S + RVC_LW_IMM));

#endif

#define CLWTYPE_INSN
