require_either_extension(xlen == 32 ? EXT_ZBPBO : EXT_ZBB, EXT_ZBB);
reg_t x = xlen;
for (int i = 0; i < xlen; i++)
  if (1 & (RS1 >> (xlen-i-1))) { x = i; break; }
WRITE_RD(sext_xlen(x));
