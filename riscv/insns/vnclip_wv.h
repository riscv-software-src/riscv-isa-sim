// vnclip: vd[i] = clip(round(vs2[i] + rnd) >> vs1[i])
VI_VV_LOOP_NARROW
({
  VRM xrm = P.VU.get_vround_mode();
  int64_t int_max = INT64_MAX >> (64 - P.VU.vsew);
  int64_t int_min = INT64_MIN >> (64 - P.VU.vsew);
  int128_t result = vs2;
  unsigned shift = vs1 & ((sew * 2) - 1);

  // rounding
  INT_ROUNDING(result, xrm, shift);

  result = result >> shift;

  // saturation
  if (result < int_min) {
    result = int_min;
    P_SET_OV(1);
  } else if (result > int_max) {
    result = int_max;
    P_SET_OV(1);
  }

  vd = result;
})
