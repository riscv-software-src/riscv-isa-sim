#ifdef BODY
require_extension(EXT_ZCA);
MMU.store<uint32_t>(RVC_SP + RVC_SWSP_IMM, RVC_RS2);

#endif

#define CSSSWTYPE_INSN
