// vsadd.vi vd, vs2 simm5
VI_CHECK_SSS(false);
VI_LOOP_BASE
bool sat = false;
switch (sew) {
case e8: {
  VI_PARAMS(e8);
  std::tie(vd, sat) = sat_add<int8_t, uint8_t>(vs2, vsext(simm5, sew));
  break;
}
case e16: {
  VI_PARAMS(e16);
  std::tie(vd, sat) = sat_add<int16_t, uint16_t>(vs2, vsext(simm5, sew));
  break;
}
case e32: {
  VI_PARAMS(e32);
  std::tie(vd, sat) = sat_add<int32_t, uint32_t>(vs2, vsext(simm5, sew));
  break;
}
default: {
  VI_PARAMS(e64);
  std::tie(vd, sat) = sat_add<int64_t, uint64_t>(vs2, vsext(simm5, sew));
  break;
}
}
P_SET_OV(sat);
VI_LOOP_END
