// RV32Zbb contains zext.h but not general pack
require(((xlen == 32) && (insn.rs2() == 0) && p->extension_enabled(EXT_ZBB))
  || p->extension_enabled(EXT_ZPN)
  || p->extension_enabled(EXT_ZBKB)
  || p->extension_enabled(EXT_XZBP)
  || p->extension_enabled(EXT_XZBE)
  || p->extension_enabled(EXT_XZBF)
  || ((xlen == 64) && p->extension_enabled(EXT_XZBM)));
reg_t lo = zext_xlen(RS1 << (xlen/2)) >> (xlen/2);
reg_t hi = zext_xlen(RS2 << (xlen/2));
WRITE_RD(sext_xlen(lo | hi));
