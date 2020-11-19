require_rv64;
require_extension('B');

// transpose of rs2
int64_t rs2t = RS2;
for (int i = 0; i < 3; i++) {
  rs2t = (rs2t & 0xFFFF00000000FFFFLL) | ((rs2t & 0x0000FFFF00000000LL) >> 16) | ((rs2t & 0x00000000FFFF0000LL) << 16);
  rs2t = (rs2t & 0xFF0000FFFF0000FFLL) | ((rs2t & 0x00FF000000FF0000LL) >>  8) | ((rs2t & 0x0000FF000000FF00LL) <<  8);
  rs2t = (rs2t & 0xF00FF00FF00FF00FLL) | ((rs2t & 0x0F000F000F000F00LL) >>  4) | ((rs2t & 0x00F000F000F000F0LL) <<  4);
  rs2t = (rs2t & 0xC3C3C3C3C3C3C3C3LL) | ((rs2t & 0x3030303030303030LL) >>  2) | ((rs2t & 0x0C0C0C0C0C0C0C0CLL) <<  2);
  rs2t = (rs2t & 0x9999999999999999LL) | ((rs2t & 0x4444444444444444LL) >>  1) | ((rs2t & 0x2222222222222222LL) <<  1);
}

int64_t rs1 = RS1;
uint8_t u[8]; // rows of rs1
uint8_t v[8]; // cols of rs2

for (int i = 0; i < 8; i++) {
  u[i] = rs1 >> (i*8);
  v[i] = rs2t >> (i*8);
}

uint64_t x = 0;
for (int i = 0; i < 64; i++) {
  if (popcount(u[i / 8] & v[i % 8]) & 1)
    x |= 1LL << i;
}

WRITE_RD(sext_xlen(x));
