require_rv64;
WRITE_RD(MMU.load<uint32_t>(RS1 + insn.i_imm()));
