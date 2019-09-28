// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << P.VU.vsew);
VI_VVXI_LOOP_NARROW
({
  uint64_t result = vs2_u;
  uint64_t unsigned_shift_amount = (uint64_t)(zimm5 & ((sew * 2) - 1));
  // The immediate form supports shift amounts up to 31 only.
  unsigned_shift_amount = min(unsigned_shift_amount, 31);

  // rounding
  INT_ROUNDING(result, xrm, unsigned_shift_amount);

  // unsigned shifting to rs1
  result = (vzext(result, sew * 2)) >> unsigned_shift_amount;

  // saturation
  if (result & (uint64_t)(-1ll << sew)) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
})
