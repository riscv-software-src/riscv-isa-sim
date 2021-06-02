require_extension(EXT_ZBC);
reg_t a = zext_xlen(RS1), b = zext_xlen(RS2), x = 0;
for (int i = 0; i < xlen; i++)
  if ((b >> i) & 1)
    x ^= a << i;
WRITE_RD(sext_xlen(x));
