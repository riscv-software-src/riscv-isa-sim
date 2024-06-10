require_extension(EXT_ZCA);
require((xlen == 64) || p->extension_enabled(EXT_ZCMLSD));
require(insn.rvc_rd() != 0);

if (xlen == 32) {
  WRITE_RD_PAIR(MMU.load<int64_t>(RVC_SP + insn.rvc_ldsp_imm()));
} else {
  WRITE_RD(MMU.load<int64_t>(RVC_SP + insn.rvc_ldsp_imm()));
}
