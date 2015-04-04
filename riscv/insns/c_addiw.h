require_extension('C');
require_rv64;
WRITE_RD(sext32(RVC_RS2 + insn.rvc_imm()));
