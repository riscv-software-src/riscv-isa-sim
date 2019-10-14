// vnclip: vd[i] = clip(round(vs2[i] + rnd) >> simm)
VRM xrm = P.VU.get_vround_mode();
int64_t int_max = (1 << (P.VU.vsew - 1)) - 1;
int64_t int_min = -(1 << (P.VU.vsew - 1));
VI_VVXI_LOOP_NARROW
({
  int64_t result = vs2;
  uint64_t unsigned_shift_amount = (uint64_t)(zimm5 & ((sew * 2) - 1));
  // The immediate form supports shift amounts up to 31 only.
  unsigned_shift_amount = std::min(unsigned_shift_amount, 31ull);

  // rounding
  INT_ROUNDING(result, xrm, unsigned_shift_amount);

  result = result >> unsigned_shift_amount;

  // saturation
  if (result < int_min) {
    result = int_min;
    P.VU.vxsat = 1;
  } else if (result > int_max) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
})
