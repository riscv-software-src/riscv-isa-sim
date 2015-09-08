require_extension('C');
require(insn.rvc_addi16sp_imm() != 0);
WRITE_REG(X_SP, sext_xlen(RVC_SP + insn.rvc_addi16sp_imm()));
