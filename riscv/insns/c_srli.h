require_extension('C');
require(insn.rvc_zimm() < xlen);
require((xlen < 128) ? (insn.rvc_zimm() > 0) : 1);
WRITE_RVC_RS1S(sext_xlen(zext_xlen(RVC_RS1S) >> insn.rvc_zimm()));
