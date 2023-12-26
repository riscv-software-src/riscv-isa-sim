// Zbkb contains unzip but not general unshfli
require_rv32;
require(((insn.rs2() == (xlen / 2 - 1)) && p->extension_enabled(EXT_ZBKB)));
require(SHAMT < (xlen/2));
reg_t x = RS1;
int shamt = SHAMT & ((xlen-1) >> 1);
if (shamt &  1) x = (x & 0x9999999999999999LL) | ((x & 0x4444444444444444LL) >>  1) | ((x & 0x2222222222222222LL) <<  1);
if (shamt &  2) x = (x & 0xC3C3C3C3C3C3C3C3LL) | ((x & 0x3030303030303030LL) >>  2) | ((x & 0x0C0C0C0C0C0C0C0CLL) <<  2);
if (shamt &  4) x = (x & 0xF00FF00FF00FF00FLL) | ((x & 0x0F000F000F000F00LL) >>  4) | ((x & 0x00F000F000F000F0LL) <<  4);
if (shamt &  8) x = (x & 0xFF0000FFFF0000FFLL) | ((x & 0x00FF000000FF0000LL) >>  8) | ((x & 0x0000FF000000FF00LL) <<  8);
if (shamt & 16) x = (x & 0xFFFF00000000FFFFLL) | ((x & 0x0000FFFF00000000LL) >> 16) | ((x & 0x00000000FFFF0000LL) << 16);
WRITE_RD(sext_xlen(x));
