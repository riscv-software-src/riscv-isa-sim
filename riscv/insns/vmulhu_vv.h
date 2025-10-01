// vmulhu vd, vs2, vs1
require(p->extension_enabled('V') || P.VU.vsew < e64);

VI_VV_ULOOP
({
  vd = ((uint128_t)vs2 * vs1) >> sew;
})
