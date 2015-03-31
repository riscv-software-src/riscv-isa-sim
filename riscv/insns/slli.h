if (SHAMT >= xlen)
  throw trap_illegal_instruction();
WRITE_RD(sext_xlen(RS1 << SHAMT));
