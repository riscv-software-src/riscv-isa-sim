require_rv64;
require_extension('P');
reg_t x = 32 - 1;
reg_t msb = (RS1 >> (32 - 1)) & 1;
for (int i = 0; i < 32 - 1; i++) {
  if (msb != ((RS1 >> (32 - i - 2)) & 1)) {
    x = i;
    break;
  }
}
WRITE_RD(sext_xlen(x));
