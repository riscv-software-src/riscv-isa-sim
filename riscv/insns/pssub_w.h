require_rv64;
P_RD_RS1_RS2_LOOP(32,32,32, {
  bool sat;
  std::tie(p_rd, sat) = (sat_sub<int32_t, uint32_t>(p_rs1, p_rs2));
  if (sat)
    P.set_vxsat();
}
)
