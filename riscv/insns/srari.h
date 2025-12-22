require_extension('P');
require_rv32;
WRITE_RD(insn.shamtw() ? ((sext_xlen(RS1) >> insn.shamtw()) + ((sext_xlen(RS1) >> (insn.shamtw() - 1)) & 1)) : RS1);