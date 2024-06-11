require_extension(EXT_ZCA);
require(insn.rvc_rd() != 0);
WRITE_RD(MMU.load<int64_t>(RVC_SP + insn.rvc_ldsp_imm()));
