int sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_LOOP(16, 16, {
  if (sshamt < 0) {
    p_rd = (uint32_t)(uint16_t)p_rs1 >> std::min(-sshamt, 16);
  } else {
    uint32_t shx = (uint32_t)(uint16_t)p_rs1 << std::min(sshamt, 16);
    if (shx > UINT16_MAX) {
      P.set_vxsat();
      p_rd = (uint16_t)UINT16_MAX;
    } else {
      p_rd = (uint16_t)shx;
    }
  }
})

