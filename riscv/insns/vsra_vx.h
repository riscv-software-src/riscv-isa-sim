// vsra.vx vd, vs2, rs1
VI_VX_LOOP
({
  vd = vs2 >> (rs1 & (sew - 1));
})
VI_CHECK_1905
