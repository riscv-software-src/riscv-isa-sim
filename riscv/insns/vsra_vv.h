#ifdef BODY
// vsra.vv  vd, vs2, vs1
VI_VV_LOOP
({
  vd = vs2 >> (vs1 & (sew - 1));
})

#endif