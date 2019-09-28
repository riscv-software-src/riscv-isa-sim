// vnclipu: vd[i] = clip(round(vs2[i] + rnd) >> rs1[i])
VRM xrm = P.VU.get_vround_mode();
uint64_t int_max = ~(-1ll << P.VU.vsew);
VI_VVXI_LOOP_NARROW
({
  uint64_t result = vs2;
  // The low log2(2*SEW) bits of the vector or scalar shift amount value are used
  // (e.g., the low 6 bits for a SEW=64-bit to SEW=32-bit narrowing operation).
  uint64_t unsigned_shift_amount = (uint64_t)(rs1 & ((sew * 2) - 1));

  // rounding
  INT_ROUNDING(result, xrm, unsigned_shift_amount);

  result = (vzext(result, sew * 2)) >> unsigned_shift_amount;

  // saturation
  if (result & (uint64_t)(-1ll << sew)) {
    result = int_max;
    P.VU.vxsat = 1;
  }

  vd = result;
})
