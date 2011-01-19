if(xpr64)
{
  int64_t a = RS1;
  int64_t b = RS2;
  RD = (int128_t(a) * int128_t(b)) >> 64;
}
else
  RD = sext32((sreg_t(RS1) * sreg_t(RS2)) >> 32);
