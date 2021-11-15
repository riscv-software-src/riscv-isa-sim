// vwmulsu.vx vd, vs2, rs1
VI_CHECK_DSS(false);
VI_VX_LOOP_WIDEN
({
  switch(p->VU.vsew) {
  case e8:
    p->VU.elt<uint16_t>(rd_num, i, true) = (int16_t)(int8_t)vs2 * (int16_t)(uint8_t)rs1;
    break;
  case e16:
    p->VU.elt<uint32_t>(rd_num, i, true) = (int32_t)(int16_t)vs2 * (int32_t)(uint16_t)rs1;
    break;
  default:
    p->VU.elt<uint64_t>(rd_num, i, true) = (int64_t)(int32_t)vs2 * (int64_t)(uint32_t)rs1;
    break;
  }
})
