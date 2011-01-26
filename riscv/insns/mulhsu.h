if(xpr64)
{
  int64_t a = RS1;
  uint64_t b = RS2;
  RD = (int128_t(a) * uint128_t(b)) >> 64;
}
else
  RD = sext32((sreg_t(RS1) * reg_t(RS2)) >> 32);
