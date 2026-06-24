require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
  bool ov = false;
  int8_t sshamt = P_FIELD(RS2, 0, 8);
  int64_t val = p_rs1;
  if (sshamt < 0) {
    val = (int32_t)(val >> std::min(-sshamt, 32));
  } else if (sshamt >= 32 && val != 0){
    val = val > 0 ? INT32_MAX : INT32_MIN;
    ov = true;
  } else if (val != 0) {
    int64_t tmp = (int64_t)val << sshamt;
    if (tmp > INT32_MAX) {
      val = INT32_MAX;
      ov = true;
    } else if (tmp < INT32_MIN) {
      val = INT32_MIN;
      ov = true;
    } else {
      val = tmp;
    }
  }
  p_rd = val;

  if (ov) P.set_vxsat();
})
