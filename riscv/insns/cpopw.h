require_rv64;
require_extension(EXT_ZBB);
reg_t x = 0;
for (int i = 0; i < 32; i++)
  if (1 & (RS1 >> i)) x++;
WRITE_RD(sext32(x));
