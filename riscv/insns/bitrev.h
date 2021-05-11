require_extension('P');
reg_t msb = get_field(RS2, make_mask64(0, xlen == 32 ? 5 : 6));
reg_t n = get_field(RS1, make_mask64(0, msb + 1));
reg_t rev = 0;

for (size_t i = 0; i <= msb; i++) {
  rev <<= 1;
  rev |= n & 1;
  n >>= 1;
}

WRITE_RD(sext_xlen(rev));