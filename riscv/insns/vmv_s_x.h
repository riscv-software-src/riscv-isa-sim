// vmv_s_x: vd[0] = rs1
require(STATE.VU.vsew == e8 || STATE.VU.vsew == e16 ||
        STATE.VU.vsew == e32 || STATE.VU.vsew == e64);
reg_t vl = STATE.VU.vl;
reg_t sew = STATE.VU.vsew;
reg_t rd_num = insn.rd();

if (vl > 0) {
  switch(sew) {
  case e8:
    STATE.VU.elt<uint8_t>(rd_num, 0) = RS1;
    break;
  case e16:
    STATE.VU.elt<uint16_t>(rd_num, 0) = RS1;
    break;
  case e32:
    STATE.VU.elt<uint32_t>(rd_num, 0) = RS1;
    break;
  default:
    STATE.VU.elt<uint64_t>(rd_num, 0) = RS1;
    break;
  }

  const reg_t max_len = STATE.VU.VLEN / sew;
  for (reg_t i = 1; i < max_len; ++i){
    switch(sew) {
    case e8:
      STATE.VU.elt<uint8_t>(rd_num, i) = 0;
      break;
    case e16:
      STATE.VU.elt<uint16_t>(rd_num, i) = 0;
      break;
    case e32:
      STATE.VU.elt<uint32_t>(rd_num, i) = 0;
      break;
    default:
      STATE.VU.elt<uint64_t>(rd_num, i) = 0;
      break;
    }
  }
}
