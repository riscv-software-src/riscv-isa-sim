require_extension(EXT_ZILX);
require_rv64;
// scaled unsigned-word-index doubleword load: EA = base(rs2) + (zext32(index(rs1)) << 3)
WRITE_RD(MMU.load<int64_t>(RS2 + (zext32(RS1) << 3)));
