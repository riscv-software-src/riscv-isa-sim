// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> vs1[i])
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << P.VU.vsew);
VI_VVXI_LOOP_NARROW
({

  uint64_t result = vs2_u;

// rounding
  INT_ROUNDING(result, xrm, sew);

// unsigned shifting to rs1
  uint64_t unsigned_shift_amount = (uint64_t)(vs1 & ((sew * 2) - 1));
  if (unsigned_shift_amount >= (2 * sew)) {
    result = 0;
  } else {
    result = vzext(result, sew * 2) >> unsigned_shift_amount;
  }
// saturation
  if (result & (uint64_t)(-1ll << sew)) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
})
