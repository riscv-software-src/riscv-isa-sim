require_extension('P');
sreg_t rs1 = sext32(RS1);
sreg_t sa = int64_t(RS2) << (64 - 6) >> (64 - 6);

if (sa < 0) {
  sa = -sa;
  sa = (sa == 32) ? 31 : sa;
  WRITE_RD(sext32(((rs1 >> (sa - 1)) + 1)) >> 1);
} else {
  auto res = rs1 << sa;
  P_SAT(res, 32);
  WRITE_RD(sext32(res));
}