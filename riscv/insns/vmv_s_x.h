// vmv_s_x: vd[0] = rs1
// this is wordaround implementation. The spec uses rs1
VI_LOOP_BASE

  uint64_t val = RS1;
  switch(sew) {
  case e8:
    STATE.VU.elt<uint8_t>(rd_num, i) = RS2;
    break;
  case e16:
    STATE.VU.elt<uint16_t>(rd_num, i) = RS2;
    break;
  case e32:
    STATE.VU.elt<uint32_t>(rd_num, i) = RS2;
    break;
  default:
    STATE.VU.elt<uint64_t>(rd_num, i) = RS2;
    break;
  }

  break;
VI_LOOP_END 
