require_extension(EXT_ZCA);
require((xlen == 64) || p->extension_enabled(EXT_ZCMLSD));

if (xlen == 32) {
  MMU.store<uint64_t>(RVC_RS1S + insn.rvc_ld_imm(), RVC_RS2S_PAIR);
} else {
  MMU.store<uint64_t>(RVC_RS1S + insn.rvc_ld_imm(), RVC_RS2S);
}
