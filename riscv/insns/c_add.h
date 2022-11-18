require_extension(EXT_ZCA);
require(insn.rvc_rs2() != 0);
WRITE_RD(sext_xlen(RVC_RS1 + RVC_RS2));
