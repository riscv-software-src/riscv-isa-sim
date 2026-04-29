require_extension('P');
require_rv32;
WRITE_RD(P_USAT(insn.shamtw() + 1, (sreg_t)RS1));