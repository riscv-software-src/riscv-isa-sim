// vdiv.vx vd, vs2, rs1
VI_VX_LOOP
({
  if(rs1 == 0)
    vd = -1;
  else if(vs2 == -(1 << (sew - 1)) && rs1 == -1)
    vd = vs2;
  else
    vd = vs2 / rs1;
})
