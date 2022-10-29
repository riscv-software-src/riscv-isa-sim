require_rv64;
WRITE_RD(MMU.load<int64_t>(RS1 + insn.i_imm()));
