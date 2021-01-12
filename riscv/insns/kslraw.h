require_extension('P');
sreg_t rs1 = sext_xlen(RS1);
sreg_t sa = int64_t(RS2) << (64 - 6) >> (64 - 6);

if (sa < 0) {
  sa = -sa;
  sa = (sa == 32) ? 31 : sa;
  WRITE_RD(sext32(rs1 >> sa));
} else {
  bool sat = false;
  WRITE_RD((sat_shl<int32_t, uint32_t>(rs1, sa, sat)));
  P.VU.vxsat |= sat;
}