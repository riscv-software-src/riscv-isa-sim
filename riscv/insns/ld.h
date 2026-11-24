require((xlen == 64) || p->extension_enabled(EXT_ZILSD));

if (xlen == 32) {
  WRITE_RD_PAIR(MMU.load<int64_t>(RS1 + insn.i_imm()));
} else {
  WRITE_RD(MMU.load<int64_t>(RS1 + insn.i_imm()));
}

