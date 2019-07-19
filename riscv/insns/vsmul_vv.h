// vsmul: Signed saturating and rounding fractional multiply
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = (uint64_t(1) << (P.VU.vsew - 1)) - 1;
uint64_t int_min = - (1 << (P.VU.vsew - 1));
uint64_t sign_mask = uint64_t(1) << (P.VU.vsew - 1);

VI_VV_ULOOP
({
  uint64_t vs1_sign;
  uint64_t vs2_sign;
  uint64_t result_sign;

  vs1_sign = vs1 & sign_mask;
  vs2_sign = vs2 & sign_mask;
  bool overflow = vs1 == vs2 && vs1 == int_min;

  uint128_t result = (uint128_t)vs1 * (uint128_t)vs2;
  result &= ((uint128_t)1llu << ((sew * 2) - 2)) - 1;
  result_sign = (vs1_sign ^ vs2_sign) & sign_mask;
  // rounding
  INT_ROUNDING(result, xrm, sew - 1);
  // unsigned shifting
  result = result >> (sew - 1);

  // saturation
  if (overflow) {
    result = int_max;
    P.VU.vxsat = 1;
  } else {
    result |= result_sign;
  }
  vd = result;
})
