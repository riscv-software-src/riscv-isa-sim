require_extension(EXT_ZCA);
require((xlen == 64) || p->extension_enabled(EXT_ZCMLSD));

if (xlen == 32) {
  MMU.store<uint64_t>(RVC_SP + insn.rvc_sdsp_imm(), RVC_RS2_PAIR);
} else {
  MMU.store<uint64_t>(RVC_SP + insn.rvc_sdsp_imm(), RVC_RS2);
}
