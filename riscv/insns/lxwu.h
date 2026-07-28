require_extension(EXT_ZILX);
require_rv64;
// unscaled indexed load: EA = base(rs2) + index(rs1)
WRITE_RD(MMU.load<uint32_t>(sext_xlen(RS2 + RS1)));
