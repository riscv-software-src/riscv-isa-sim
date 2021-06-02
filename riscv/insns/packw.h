// RV64Zbb contains zext.h but not general packw
if (insn.rs2() == 0)
  require_extension(EXT_ZBB);
else
  require_extension('B');

require_rv64;
reg_t lo = zext32(RS1 << 16) >> 16;
reg_t hi = zext32(RS2 << 16);
WRITE_RD(sext32(lo | hi));
