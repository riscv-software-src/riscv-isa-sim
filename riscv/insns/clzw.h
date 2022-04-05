require_rv64;
require_extension(EXT_ZBB);
reg_t x = 32;
for (int i = 0; i < 32; i++)
  if (1 & (RS1 >> (31-i))) { x = i; break; }
WRITE_RD(sext32(x));
