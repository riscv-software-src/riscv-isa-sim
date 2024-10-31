#ifdef BODY
require_extension(EXT_ZCA);
MMU.store<uint32_t>(RVC_RS1S + RVC_LW_IMM, RVC_RS2S);

#endif

#define CSWTYPE_INSN
