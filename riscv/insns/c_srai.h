require_extension('C');
require(insn.rvc_zimm() < xlen);
WRITE_RVC_RS1S(sext_xlen(sext_xlen(RVC_RS1S) >> insn.rvc_zimm()));
