require_extension(EXT_ZCA);
require((xlen == 64) || p->extension_enabled(EXT_ZCMLSD));

if (xlen == 32) {
  WRITE_RVC_RS2S_PAIR(MMU.load<int64_t>(RVC_RS1S + insn.rvc_ld_imm()));
} else {
  WRITE_RVC_RS2S(MMU.load<int64_t>(RVC_RS1S + insn.rvc_ld_imm()));
}
