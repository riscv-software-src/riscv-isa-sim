require_extension(EXT_ZILX);
require_rv64;
// scaled unsigned-word-index byte load: EA = base(rs2) + (zext32(index(rs1)) << 0)
WRITE_RD(MMU.load<uint8_t>(RS2 + zext32(RS1)));
