require_extension('C');
require(insn.rvc_zimm() < xlen && insn.rvc_zimm() > 0);
WRITE_RVC_RS1S(sext_xlen(sext_xlen(RVC_RS1S) >> insn.rvc_zimm()));
