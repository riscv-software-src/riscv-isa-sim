require_extension('P');
require_rv64;
WRITE_RD(P_USAT(insn.shamtd() + 1, (sreg_t)RS1));
