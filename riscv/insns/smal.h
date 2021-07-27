require_extension('P');
sreg_t res = 0;
if (xlen == 32) {
  res = RS1_PAIR;
  res += sext_xlen(P_SH(RS2, 0) * P_SH(RS2, 1));
  WRITE_RD_PAIR(res);
} else {
  res = sext_xlen(P_SH(RS2, 0) * P_SH(RS2, 1)) +
        sext_xlen(P_SH(RS2, 2) * P_SH(RS2, 3)) + RS1;
  WRITE_RD(res);
}