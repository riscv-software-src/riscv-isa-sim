require_extension(EXT_ZILX);
// unscaled indexed load: EA = base(rs2) + index(rs1)
WRITE_RD(MMU.load<int16_t>(sext_xlen(RS2 + RS1)));
