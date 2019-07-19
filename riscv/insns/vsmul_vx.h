// vsmul
VRM xrm = P.VU.get_vround_mode();
uint128_t int_max = (uint64_t(1) << (P.VU.vsew - 1)) - 1;
uint128_t int_min = - (1 << (P.VU.vsew - 1));
uint128_t sign_mask = uint64_t(1) << (P.VU.vsew - 1);

VI_VX_ULOOP
({
  uint128_t rs1_sign;
  uint128_t vs2_sign;
  uint128_t result_sign;

  rs1_sign = rs1 & sign_mask;
  vs2_sign = vs2 & sign_mask;
  bool overflow = rs1 == vs2 && rs1 == int_min;

  uint128_t result = (uint128_t)rs1 * (uint128_t)vs2;
  result &= ((uint128_t)1llu << ((sew * 2) - 2)) - 1;
  result_sign = (rs1_sign ^ vs2_sign) & sign_mask;
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
