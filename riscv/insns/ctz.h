require_extension('B');
reg_t x = xlen;
for (int i = 0; i < xlen; i++)
  if (1 & (RS1 >> i)) { x = i; break; }
WRITE_RD(sext_xlen(x));
