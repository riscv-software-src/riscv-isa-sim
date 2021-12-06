// vmadc.vim vd, vs2, simm5, v0
VI_XI_LOOP_CARRY
({
  res = (((op_mask & simm5) + (op_mask & vs2) + carry) >> sew) & 0x1u;
})
