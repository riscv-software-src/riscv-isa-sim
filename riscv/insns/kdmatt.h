require_extension('P');
sreg_t res;
sreg_t aop = P_SH(RS1, 1);
sreg_t bop = P_SH(RS2, 1);

if ((INT16_MIN != aop) | (INT16_MIN != bop)) {
  res = aop * bop;
  res <<= 1;
} else {
  res = INT32_MAX;
  P_SET_OV(1);
}

res += sext32(RD);
P_SAT(res, 32);
WRITE_RD(sext32(res));