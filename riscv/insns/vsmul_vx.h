// vsmul.vx vd, vs2, rs1
VI_VX_LOOP
({
  VRM xrm = P.VU.get_vround_mode();
  int64_t int_max = INT64_MAX >> (64 - P.VU.vsew);
  int64_t int_min = INT64_MIN >> (64 - P.VU.vsew);

  bool overflow = rs1 == vs2 && rs1 == int_min;
  int128_t result = (int128_t)rs1 * (int128_t)vs2;

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
