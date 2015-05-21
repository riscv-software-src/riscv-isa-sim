require_extension('C');
require(insn.rvc_imm() < xlen);
WRITE_RD(sext_xlen(sext_xlen(RVC_RS1) >> insn.rvc_imm()));
