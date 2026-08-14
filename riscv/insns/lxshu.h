require_extension(EXT_ZILX);
// scaled indexed halfword load: EA = base(rs2) + (index(rs1) << 1)
WRITE_RD(MMU.load<uint16_t>(sext_xlen(RS2 + (RS1 << 1))));
