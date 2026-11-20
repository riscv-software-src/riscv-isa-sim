// vssubu.vx vd, vs2, rs1
VI_CHECK_SSS(false);
VI_LOOP_BASE
bool sat = false;

switch (sew) {
case e8: {
  VX_U_PARAMS(e8);
  std::tie(vd, sat) = (sat_subu<uint8_t>(vs2, rs1));
  break;
}
case e16: {
  VX_U_PARAMS(e16);
  std::tie(vd, sat) = (sat_subu<uint16_t>(vs2, rs1));
  break;
}
case e32: {
  VX_U_PARAMS(e32);
  std::tie(vd, sat) = (sat_subu<uint32_t>(vs2, rs1));
  break;
}
default: {
  VX_U_PARAMS(e64);
  std::tie(vd, sat) = (sat_subu<uint64_t>(vs2, rs1));
  break;
}
}
P_SET_OV(sat);
VI_LOOP_END
