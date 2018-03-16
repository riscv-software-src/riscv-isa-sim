require_extension('C');
require(insn.rvc_zimm() < xlen);
require((xlen < 128) ? (insn.rvc_zimm() > 0) : 1);
WRITE_RD(sext_xlen(RVC_RS1 << insn.rvc_zimm()));
