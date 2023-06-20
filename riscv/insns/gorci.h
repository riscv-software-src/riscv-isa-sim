// Zbb contains orc.b but not general gorci
require(((SHAMT == 7) && p->extension_enabled(EXT_ZBB)));
require(SHAMT < xlen);
reg_t x = RS1;
int shamt = SHAMT;
if (shamt &  1) x |= ((x & 0x5555555555555555LL) <<  1) | ((x & 0xAAAAAAAAAAAAAAAALL) >>  1);
if (shamt &  2) x |= ((x & 0x3333333333333333LL) <<  2) | ((x & 0xCCCCCCCCCCCCCCCCLL) >>  2);
if (shamt &  4) x |= ((x & 0x0F0F0F0F0F0F0F0FLL) <<  4) | ((x & 0xF0F0F0F0F0F0F0F0LL) >>  4);
if (shamt &  8) x |= ((x & 0x00FF00FF00FF00FFLL) <<  8) | ((x & 0xFF00FF00FF00FF00LL) >>  8);
if (shamt & 16) x |= ((x & 0x0000FFFF0000FFFFLL) << 16) | ((x & 0xFFFF0000FFFF0000LL) >> 16);
if (shamt & 32) x |= ((x & 0x00000000FFFFFFFFLL) << 32) | ((x & 0xFFFFFFFF00000000LL) >> 32);
WRITE_RD(sext_xlen(x));
