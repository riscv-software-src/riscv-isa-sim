// vsbc.vvm vd, vs2, rs1, v0
VI_VV_LOOP_WITH_CARRY
({
  vd = (uint128_t)((op_mask & vs2) - (op_mask & vs1) - carry);
})
