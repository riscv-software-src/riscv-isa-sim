require_extension('P');
sreg_t rs1 = sext_xlen(RS1);
sreg_t sa = get_field(RS2, make_mask64(0, 5));
bool sat = false;

WRITE_RD(sext_xlen((sat_shl<int32_t, uint32_t>(rs1, sa, sat))));
P.VU.vxsat |= sat;