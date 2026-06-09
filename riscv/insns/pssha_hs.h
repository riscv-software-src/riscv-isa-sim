sreg_t sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_LOOP(16, 16, {
  if (p_rs1 == 0)
    p_rd = 0;
  else if (sshamt >= 16) {
    p_rd = (p_rs1 & 0x8000) ? INT16_MIN : INT16_MAX;
    P.set_vxsat();
  }
  else if (sshamt <= -16)
    p_rd = (p_rs1 & 0x8000) ? (int16_t)UINT16_MAX : 0;
  else
    p_rd = sshamt >= 0 ? P_SAT(16, sext32(p_rs1) << sshamt) : (p_rs1 >> -sshamt);
})
