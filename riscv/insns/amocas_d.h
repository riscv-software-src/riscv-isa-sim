require_extension(EXT_ZACAS);

if (xlen == 32) {
  // RV32: the spec defines two 32-bit comparisons. Since we're
  // loading 64-bit for memory we have to adjust for endianness.
  uint64_t comp, swap, res;

  require_align(insn.rd(), 2);
  require_align(insn.rs2(), 2);
  if (insn.rd() == 0) {
    comp = 0;
  } else if (MMU.is_target_big_endian()) {
    comp = (uint32_t)READ_REG(insn.rd() + 1) | (RD << 32);
  } else {
    comp = (uint32_t)RD | (READ_REG(insn.rd() + 1) << 32);
  }
  if (insn.rs2() == 0) {
    swap = 0;
  } else if (MMU.is_target_big_endian()) {
    swap = (uint32_t)READ_REG(insn.rs2() + 1) | (RS2 << 32);
  } else {
    swap = (uint32_t)RS2 | (READ_REG(insn.rs2() + 1) << 32);
  }
  res = MMU.amo_compare_and_swap<uint64_t>(RS1, comp, swap);
  if (insn.rd() != 0) {
    if (MMU.is_target_big_endian()) {
      WRITE_REG(insn.rd() + 1, sext32((uint32_t)res));
      WRITE_REG(insn.rd(), sext32(res >> 32));
    } else {
      WRITE_REG(insn.rd(), sext32((uint32_t)res));
      WRITE_REG(insn.rd() + 1, sext32(res >> 32));
    }
  }
 } else {
  // RV64
  WRITE_RD(MMU.amo_compare_and_swap<uint64_t>(RS1, RD, RS2));
}
