require_extension(EXT_ZACAS);
require_rv64;
require_align(insn.rd(), 2);
require_align(insn.rs2(), 2);

// The spec defines two 64-bit comparisons. Since we're loading
// 128-bit for memory we have to adjust for endianness.

uint128_t comp, swap, res;

if (insn.rd() == 0) {
  comp = 0;
} else if (MMU.is_target_big_endian()) {
  comp = READ_REG(insn.rd() + 1) | ((uint128_t)RD << 64);
} else  {
  comp = RD | ((uint128_t)READ_REG(insn.rd() + 1) << 64);
}
if (insn.rs2() == 0) {
  swap = 0;
} else if (MMU.is_target_big_endian()) {
  swap = READ_REG(insn.rs2() + 1) | ((uint128_t)RS2 << 64);
} else {
  swap = RS2 | ((uint128_t)READ_REG(insn.rs2() + 1) << 64);
}
res = MMU.amo_compare_and_swap<uint128_t>(RS1, comp, swap);
if (insn.rd() != 0) {
  if (MMU.is_target_big_endian()) {
    WRITE_REG(insn.rd(), res >> 64);
    WRITE_REG(insn.rd() + 1, res);
  } else {
    WRITE_REG(insn.rd(), res);
    WRITE_REG(insn.rd() + 1, res >> 64);
  }
}
