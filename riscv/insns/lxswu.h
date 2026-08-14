require_extension(EXT_ZILX);
require_rv64;
// scaled indexed word load: EA = base(rs2) + (index(rs1) << 2)
WRITE_RD(MMU.load<uint32_t>(sext_xlen(RS2 + (RS1 << 2))));
