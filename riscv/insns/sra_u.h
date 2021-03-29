require_extension('P');
sreg_t rs1 = sext_xlen(RS1);
reg_t sa = get_field(RS2, make_mask64(0, xlen == 32 ? 5 : 6));

if (sa > 0) {
  WRITE_RD(sext_xlen(((rs1 >> (sa - 1)) + 1) >> 1));
} else {
  WRITE_RD(sext_xlen(rs1));
}