// RV32Zbb contains zext.h but not general pack
require(((xlen == 32) && (insn.rs2() == 0) && p->extension_enabled(EXT_ZBB))
  || p->extension_enabled(EXT_ZBKB));
reg_t lo = zext_xlen(RS1 << (xlen/2)) >> (xlen/2);
reg_t hi = zext_xlen(RS2 << (xlen/2));
WRITE_RD(sext_xlen(lo | hi));
