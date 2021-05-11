require_extension('P');
sreg_t res;
sreg_t aop = P_SH(RS1, 0);
sreg_t bop = P_SH(RS2, 0);
if ((INT16_MIN != aop) | (INT16_MIN != bop)) {
  res = aop * bop;
  res >>= 15;
} else {
  res = INT16_MAX;
  P_SET_OV(1);
}
WRITE_RD(sext_xlen((int16_t)res));