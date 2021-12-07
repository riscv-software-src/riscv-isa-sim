// vadc.vim vd, vs2, simm5, v0
VI_XI_LOOP_WITH_CARRY
({
  vd = (uint128_t)((op_mask & simm5) + (op_mask & vs2) + carry);
})
