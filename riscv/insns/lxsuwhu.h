require_extension(EXT_ZILX);
require_rv64;
// scaled unsigned-word-index halfword load: EA = base(rs2) + (zext32(index(rs1)) << 1)
WRITE_RD(MMU.load<uint16_t>(RS2 + (zext32(RS1) << 1)));
