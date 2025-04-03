// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VI_VI_LOOP_NARROW
({
  VRM xrm = P.VU.get_vround_mode();
  uint64_t uint_max = UINT64_MAX >> (64 - P.VU.vsew);
  uint64_t sign_mask = UINT64_MAX << P.VU.vsew;
  uint128_t result = vs2_u;
  unsigned shift = zimm5 & ((sew * 2) - 1);

  // rounding
  INT_ROUNDING(result, xrm, shift);

  // unsigned shifting to rs1
  result = result >> shift;

  // saturation
  if (result & sign_mask) {
    result = uint_max;
    P_SET_OV(1);
  }

  vd = result;
})
