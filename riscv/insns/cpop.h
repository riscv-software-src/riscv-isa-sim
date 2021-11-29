require_extension(EXT_ZBB);
reg_t x = 0;
for (int i = 0; i < xlen; i++)
  if (1 & (RS1 >> i)) x++;
WRITE_RD(sext_xlen(x));
