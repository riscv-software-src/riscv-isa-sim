// vmulhu vd ,vs2, vs1
VI_VV_ULOOP
({
  vd = ((uint128_t)vs2 * vs1) >> sew;
})
