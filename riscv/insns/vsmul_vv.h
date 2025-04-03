// vsmul.vv vd, vs2, vs1
VI_VV_LOOP
({
  VRM xrm = P.VU.get_vround_mode();
  int64_t int_max = INT64_MAX >> (64 - P.VU.vsew);
  int64_t int_min = INT64_MIN >> (64 - P.VU.vsew);

  bool overflow = vs1 == vs2 && vs1 == int_min;
  int128_t result = (int128_t)vs1 * (int128_t)vs2;

  // rounding
  INT_ROUNDING(result, xrm, sew - 1);

  // remove guard bits
  result = result >> (sew - 1);

  // max saturation
  if (overflow) {
    result = int_max;
    P_SET_OV(1);
  }

  vd = result;
})
