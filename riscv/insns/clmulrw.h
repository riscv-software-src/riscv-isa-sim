require_extension('B');
reg_t a = zext32(RS1), b = zext32(RS2), x = 0;
for (int i = 0; i < 32; i++)
  if ((b >> i) & 1)
    x ^= a >> (31-i);
WRITE_RD(sext32(x));
