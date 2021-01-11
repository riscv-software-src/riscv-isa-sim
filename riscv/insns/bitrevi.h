require_extension('P');
reg_t msb = xlen == 32 ? insn.p_imm5() : insn.p_imm6();
reg_t n = get_field(RS1, make_mask64(0, msb + 1));
reg_t rev = 0;

for (size_t i = 0; i <= msb; i++) {
  rev <<= 1;
  rev |= n & 1;
  n >>= 1;
}

WRITE_RD(rev);