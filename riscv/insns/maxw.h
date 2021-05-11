require_extension('P');
int32_t rs1w = P_W(RS1, 0);
int32_t rs2w = P_W(RS2, 0);
WRITE_RD(sext_xlen(rs1w >= rs2w ? rs1w : rs2w));