// vdiv.vv vd, vs2, vs1
VI_VV_LOOP
({
  if (vs1 == 0)
    vd = -1;
  else if (vs2 == (INT64_MIN >> (64 - sew)) && vs1 == -1)
    vd = vs2;
  else
    vd = vs2 / vs1;
})
