// vmul vd, vs2, rs1
VI_VX_LOOP
({
  vd = vs2 * rs1;
})
