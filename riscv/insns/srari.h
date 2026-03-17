require_extension('P');
require_rv64;
WRITE_RD(insn.shamtd() ? ((sext_xlen(RS1) >> insn.shamtd()) + ((sext_xlen(RS1) >> (insn.shamtd() - 1)) & 1)) : RS1);
