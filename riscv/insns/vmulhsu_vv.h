// vmulhsu.vv vd, vs2, vs1
require(p->extension_enabled('V') || P.VU.vsew < e64);

VI_VV_SU_LOOP({
  vd = ((int128_t)vs2 * (uint128_t)vs1) >> sew;
})
