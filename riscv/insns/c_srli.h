require_extension(EXT_ZCA);
require(insn.rvc_zimm() < xlen);
WRITE_RVC_RS1S(sext_xlen(zext_xlen(RVC_RS1S) >> insn.rvc_zimm()));
