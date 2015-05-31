require_extension('C');
if (xlen == 32) {
  switch ((insn.bits() >> 5) & 3) {
    case 0: WRITE_RVC_RS2S(sext_xlen(RVC_RS1S + insn.rvc_simm3())); // c.addin
    case 1: WRITE_RVC_RS2S(sext_xlen(RVC_RS1S ^ insn.rvc_simm3())); // c.xorin
    case 2: WRITE_RVC_RS2S(sext_xlen(RVC_RS1S | insn.rvc_simm3())); // c.orin
    case 3: WRITE_RVC_RS2S(sext_xlen(RVC_RS1S & insn.rvc_simm3())); // c.andin
  }
} else {
  require(insn.rvc_rd() != 0);
  require(insn.rvc_imm() < 32);
  WRITE_RD(sext32(RVC_RS1 << insn.rvc_imm()));
}
