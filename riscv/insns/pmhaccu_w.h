require_rv64;
P_RD_RS1_RS2_ULOOP(32,32,32, {
  reg_t mres = zext(p_rs1,64) * zext(p_rs2,64);
  p_rd += mres>>32;
})