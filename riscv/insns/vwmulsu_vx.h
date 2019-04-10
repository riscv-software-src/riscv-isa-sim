// vwmulsu vd, vs2, vs1
V_WIDE_CHECK;
VI_VX_LOOP
({
  V_LOOP_ELEMENT_SKIP;
  switch(STATE.VU.vsew) {
  case e8:
    STATE.VU.elt<uint16_t>(rd_num, i) = (int16_t)(int8_t)vs2 * (int16_t)(uint8_t)rs1;
    break;
  case e16:
    STATE.VU.elt<uint32_t>(rd_num, i) = (int32_t)(int16_t)vs2 * (int32_t)(uint16_t)rs1;
    break;
  default:
    STATE.VU.elt<uint64_t>(rd_num, i) = (int64_t)(int32_t)vs2 * (int64_t)(uint32_t)rs1;
    break;
  }
})
