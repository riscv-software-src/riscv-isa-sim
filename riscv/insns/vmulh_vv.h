// vmulh vd, vs2, vs1
VI_VV_LOOP
({
  vd = ((int128_t)vs2 * vs1) >> sew;
})
