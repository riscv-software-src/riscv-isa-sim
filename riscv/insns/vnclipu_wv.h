// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> vs1[i])
VRM xrm = P.VU.get_vround_mode();
uint64_t uint_max = UINT64_MAX >> (64 - P.VU.vsew);
uint64_t sign_mask = UINT64_MAX << P.VU.vsew;
VI_VVXI_LOOP_NARROW
({
  uint128_t result = vs2_u;
  unsigned shift = vs1 & ((sew * 2) - 1);

  // rounding
  INT_ROUNDING(result, xrm, shift);

  result = result >> shift;

  // saturation
  if (result & sign_mask) {
    result = uint_max;
    P.VU.vxsat = 1;
  }

  vd = result;
}, true)
