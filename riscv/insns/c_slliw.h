require_extension('C');
require_rv64;
require(insn.rvc_rd() != 0);
require(insn.rvc_imm() < 32);
WRITE_RD(sext32(RVC_RS1 << insn.rvc_imm()));
