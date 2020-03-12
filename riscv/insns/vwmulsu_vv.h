// vwmulsu.vv vd, vs2, vs1
VI_CHECK_DSS(true);
VI_VV_LOOP_WIDEN
({
  switch(P.VU.vsew) {
  case e8:
    P.VU.elt<uint16_t>(rd_num, i, true) = (int16_t)(int8_t)vs2 * (int16_t)(uint8_t)vs1;
    break;
  case e16:
    P.VU.elt<uint32_t>(rd_num, i, true) = (int32_t)(int16_t)vs2 * (int32_t)(uint16_t)vs1;
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i, true) = (int64_t)(int32_t)vs2 * (int64_t)(uint32_t)vs1;
    break;
  }
})
