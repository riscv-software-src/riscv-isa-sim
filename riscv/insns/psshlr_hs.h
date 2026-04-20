sreg_t sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_LOOP(16, 16, {
  if (sshamt < 0) {
    uint32_t shx;
    if (sshamt < -16)
      shx = 0;
    else if (sshamt == -16)
      shx = ((uint16_t)p_rs1 >> 15) & 1;
    else
      shx = ((uint32_t)(uint16_t)p_rs1 << 1) >> (-sshamt);
    p_rd = (uint16_t)((shx + 1) >> 1);
  } else {
    uint32_t shx = (sshamt >= 16) ? ((uint32_t)(uint16_t)p_rs1 << 16) : ((uint32_t)(uint16_t)p_rs1 << sshamt);
    if (shx > 0xFFFF) {
      P.VU.vxsat->write(1);
      p_rd = 0xFFFF;
    } else {
      p_rd = (uint16_t)shx;
    }
  }
})

