require((xlen == 64) || p->extension_enabled(EXT_ZILSD));

if (xlen == 32) {
  uint64_t loaded_val = MMU.load<int64_t>(RS1 + insn.i_imm());
  WRITE_RD_PAIR(loaded_val);
} else {
  WRITE_RD(MMU.load<int64_t>(RS1 + insn.i_imm()));
}

