require_extension(EXT_ZILX);
require_rv64;
// scaled indexed doubleword load: EA = base(rs2) + (index(rs1) << 3)
WRITE_RD(MMU.load<int64_t>(sext_xlen(RS2 + (RS1 << 3))));
