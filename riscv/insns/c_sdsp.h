#ifdef BODY
require_extension(EXT_ZCA);
require((xlen == 64) || p->extension_enabled(EXT_ZCLSD));

if (xlen == 32) {
  MMU.store<uint64_t>(RVC_SP + RVC_SDSP_IMM, RVC_RS2_PAIR);
} else {
  MMU.store<uint64_t>(RVC_SP + RVC_SDSP_IMM, RVC_RS2);
}

#endif

#define CSSSDTYPE_INSN
