require_rv64;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_LOOP(32, 32, {
  if (sshamt < 0) {
    if (sshamt <= -32)
      p_rd = 0;
    else
      p_rd = (uint32_t)p_rs1 >> (-sshamt);
  } else {
    uint64_t shx = (sshamt >= 32) ? ((uint64_t)(uint32_t)p_rs1 << 32) : ((uint64_t)(uint32_t)p_rs1 << sshamt);
    if (shx > 0xFFFFFFFFULL) {
      P.VU.vxsat->write(1);
      p_rd = 0xFFFFFFFF;
    } else {
      p_rd = (uint32_t)shx;
    }
  }
})

