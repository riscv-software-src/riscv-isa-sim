require_extension(EXT_ZILX);
// scaled indexed byte load: EA = base(rs2) + (index(rs1) << 0)
WRITE_RD(MMU.load<int8_t>(sext_xlen(RS2 + RS1)));
