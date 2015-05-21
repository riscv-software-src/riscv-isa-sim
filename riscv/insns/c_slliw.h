require_extension('C');
require_rv64;
WRITE_RD(sext_xlen(RVC_RS1 << insn.rvc_imm()));
