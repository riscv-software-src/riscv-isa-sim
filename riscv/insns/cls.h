require_extension('P');
reg_t x = xlen - 1;
reg_t msb = (RS1 >> (xlen - 1)) & 1;
for (int i = 0; i < xlen - 1; i++) {
  if (msb != ((RS1 >> (xlen - i - 2)) & 1)) {
    x = i;
    break;
  }
}
WRITE_RD(sext_xlen(x));