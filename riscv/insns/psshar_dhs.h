require_rv32;
P_RD_RS1_DW_LOOP(16, 16, {
  bool ov = false;
  int8_t sshamt = P_FIELD(RS2, 0, 8);
  int32_t val = p_rs1;
  if (sshamt < 0) {
    val = (val << 1) >> std::min(-sshamt, 16);
    val = (int16_t)((val + 1) >> 1);
  } else if (sshamt >= 16 && val != 0){
    val = val > 0 ? INT16_MAX : INT16_MIN;
    ov = true;
  } else if (val != 0) {
    int32_t tmp = (int32_t)val << sshamt;
    if (tmp > INT16_MAX) {
      val = INT16_MAX;
      ov = true;
    } else if (tmp < INT16_MIN) {
      val = INT16_MIN;
      ov = true;
    } else {
      val = tmp;
    }
  }
  p_rd = val;

  if (ov) P.set_vxsat();
})
