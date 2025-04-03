require_extension(EXT_ZCA);
require(insn.rvc_rd() != 0);
WRITE_RD(MMU.load<int32_t>(RVC_SP + insn.rvc_lwsp_imm()));
