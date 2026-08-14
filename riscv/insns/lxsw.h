require_extension(EXT_ZILX);
// scaled indexed word load: EA = base(rs2) + (index(rs1) << 2)
WRITE_RD(MMU.load<int32_t>(sext_xlen(RS2 + (RS1 << 2))));
