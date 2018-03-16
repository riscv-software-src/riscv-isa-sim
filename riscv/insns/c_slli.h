require_extension('C');
require(insn.rvc_zimm() < xlen && insn.rvc_zimm() > 0);
WRITE_RD(sext_xlen(RVC_RS1 << insn.rvc_zimm()));
