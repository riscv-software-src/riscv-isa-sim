// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = p->VU.get_vround_mode();
uint64_t uint_max = UINT64_MAX >> (64 - p->VU.vsew);
uint64_t sign_mask = UINT64_MAX << p->VU.vsew;
VI_VVXI_LOOP_NARROW
({
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
}, false)
