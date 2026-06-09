require_rv32;
int sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_DW_LOOP(32, 32, {
  if (sshamt < 0) {
    uint64_t shx = ((uint64_t)(uint32_t)p_rs1 << 1) >> std::min(-sshamt, 32);
    p_rd = (uint32_t)((shx + 1) >> 1);
  } else {
    uint64_t shx = (uint64_t)(uint32_t)p_rs1 << std::min(sshamt, 32);
    if (shx > UINT32_MAX) {
      P.set_vxsat();
      p_rd = UINT32_MAX;
    } else {
      p_rd = (uint32_t)shx;
    }
  }
})

