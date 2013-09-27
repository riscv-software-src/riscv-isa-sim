if(xpr64)
{
  int64_t a = RS1;
  int64_t b = RS2;
  WRITE_RD((int128_t(a) * int128_t(b)) >> 64);
}
else
  WRITE_RD(sext32((sext32(RS1) * sext32(RS2)) >> 32));
