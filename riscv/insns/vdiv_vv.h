// vdiv.vv vd, vs2, vs1
VI_VV_LOOP
({
  if (vs1 == 0)
    vd = -1;
  else if (vs2 == -(1 << (sew - 1)) && vs1 == -1)
    vd = vs2;
  else
    vd = vs2 / vs1;
})
