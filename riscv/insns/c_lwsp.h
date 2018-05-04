require_extension('C');
require(insn.rvc_rd() != 0);
WRITE_RD(MMU.load_int32(RVC_SP + insn.rvc_lwsp_imm()));
