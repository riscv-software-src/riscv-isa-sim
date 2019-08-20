// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << P.VU.vsew);
VI_VVXI_LOOP_NARROW
({
  uint64_t result = vs2_u;
  // rounding
  INT_ROUNDING(result, xrm, sew);

  // unsigned shifting to rs1
  result = vzext(result, sew * 2) >> (zimm5 & ((sew * 2) < 32? (sew * 2) - 1: 31));

  // saturation
  if (result & (uint64_t)(-1ll << sew)) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
})
