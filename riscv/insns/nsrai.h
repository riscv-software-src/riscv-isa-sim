require_extension('P');
require_rv32;
WRITE_RD(sext32((sreg_t)P_RS1_PAIR >> insn.shamtd()));
