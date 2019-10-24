// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> rs1[i])
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << P.VU.vsew);
VI_VVXI_LOOP_NARROW
({
  uint64_t result = vs2;
  uint64_t shift = rs1 & ((sew * 2) - 1);

// rounding
  INT_ROUNDING(result, xrm, shift);

// unsigned right shift
  result = vzext(result, sew * 2) >> shift;

// saturation
  if (result & (uint64_t)(-1ll << sew)) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
})
