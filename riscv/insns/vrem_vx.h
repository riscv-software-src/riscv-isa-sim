// vrem.vx vd, vs2, rs1
VI_VX_LOOP
({
  if (rs1 == 0)
    vd = vs2;
  else if (vs2 == -(((intmax_t)1) << (sew - 1)) && rs1 == -1)
    vd = 0;
  else
    vd = vs2 % rs1;
})
