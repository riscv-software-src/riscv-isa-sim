// vsadd.vv vd, vs2, vs1
VI_CHECK_SSS(true);
VI_LOOP_BASE
bool sat = false;
switch (sew) {
case e8: {
  VV_PARAMS(e8);
  std::tie(vd, sat) = sat_add<int8_t, uint8_t>(vs2, vs1);
  break;
}
case e16: {
  VV_PARAMS(e16);
  std::tie(vd, sat) = sat_add<int16_t, uint16_t>(vs2, vs1);
  break;
}
case e32: {
  VV_PARAMS(e32);
  std::tie(vd, sat) = sat_add<int32_t, uint32_t>(vs2, vs1);
  break;
}
default: {
  VV_PARAMS(e64);
  std::tie(vd, sat) = sat_add<int64_t, uint64_t>(vs2, vs1);
  break;
}
}
P_SET_OV(sat);
VI_LOOP_END
