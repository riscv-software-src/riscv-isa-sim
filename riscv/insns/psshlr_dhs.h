require_rv32;
int sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_DW_LOOP(16, 16, {
  if (sshamt < 0) {
    uint32_t shx = ((uint32_t)(uint16_t)p_rs1 << 1) >> std::min(-sshamt, 16);
    p_rd = (uint16_t)((shx + 1) >> 1);
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

