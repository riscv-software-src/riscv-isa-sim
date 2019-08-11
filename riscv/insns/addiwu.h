require_rv64;
require_extension('B');
WRITE_RD(zext32(insn.i_imm() + RS1));
