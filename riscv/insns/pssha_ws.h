require_rv64;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
P_RD_RS1_LOOP(32, 32, {
  if (p_rs1 == 0)
    p_rd = 0;
  else if (sshamt >= 32) {
    p_rd = (p_rs1 & 0x80000000) ? INT32_MIN : INT32_MAX;
    P.set_vxsat();
  }
  else if (sshamt <= -32)
    p_rd = (p_rs1 & 0x80000000) ? (int32_t)UINT32_MAX : 0;
  else
    p_rd = sshamt >= 0 ? P_SAT(32, sext32(p_rs1) << sshamt) : (p_rs1 >> -sshamt);
}
)
