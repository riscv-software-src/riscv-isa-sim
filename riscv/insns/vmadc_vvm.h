// vmadc.vvm vd, vs2, rs1, v0
VI_VV_LOOP_CARRY
({
  res = (((op_mask & vs1) + (op_mask & vs2) + carry) >> sew) & 0x1u;
})
