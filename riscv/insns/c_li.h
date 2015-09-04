require_extension('C');
require(insn.rvc_rd() != 0);
WRITE_RD(insn.rvc_imm());
