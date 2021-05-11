require_extension('P');
int32_t rs1 = P_W(RS1, 0);

if (rs1 == INT32_MIN) {
  rs1 = INT32_MAX;
  P_SET_OV(1);
}

WRITE_RD(sext_xlen(rs1 >= 0 ? rs1 : -rs1));