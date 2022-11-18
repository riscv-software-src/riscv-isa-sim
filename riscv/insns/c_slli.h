require_extension(EXT_ZCA);
require(insn.rvc_zimm() < xlen);
WRITE_RD(sext_xlen(RVC_RS1 << insn.rvc_zimm()));
