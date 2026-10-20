// vssub.vx vd, vs2, rs1
VI_CHECK_SSS(false);
VI_LOOP_BASE
bool sat = false;

switch (sew) {
case e8: {
  VX_PARAMS(e8);
  std::tie(vd, sat) = (sat_sub<int8_t, uint8_t>(vs2, rs1));
  break;
}
case e16: {
  VX_PARAMS(e16);
  std::tie(vd, sat) = (sat_sub<int16_t, uint16_t>(vs2, rs1));
  break;
}
case e32: {
  VX_PARAMS(e32);
  std::tie(vd, sat) = (sat_sub<int32_t, uint32_t>(vs2, rs1));
  break;
}
default: {
  VX_PARAMS(e64);
  std::tie(vd, sat) = (sat_sub<int64_t, uint64_t>(vs2, rs1));
  break;
}
}
P_SET_OV(sat);
VI_LOOP_END
