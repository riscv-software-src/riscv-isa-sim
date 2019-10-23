// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << P.VU.vsew);
VI_VVXI_LOOP_NARROW
({
  uint64_t result = vs2_u;
  unsigned unsigned_shift_amount = zimm5 & ((sew * 2) - 1);
  // The immediate form supports shift amounts up to 31 only.
  unsigned_shift_amount = std::min(unsigned_shift_amount, 31u);

  // rounding
  INT_ROUNDING(result, xrm, unsigned_shift_amount);

  // unsigned shifting to rs1
  result = result >> unsigned_shift_amount;

  // saturation
  if (result & (uint64_t)(-1ll << sew)) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
}, false)
