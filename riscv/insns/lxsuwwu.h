require_extension(EXT_ZILX);
require_rv64;
// scaled unsigned-word-index word load: EA = base(rs2) + (zext32(index(rs1)) << 2)
WRITE_RD(MMU.load<uint32_t>(RS2 + (zext32(RS1) << 2)));
