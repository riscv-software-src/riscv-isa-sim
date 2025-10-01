// vmulh vd, vs2, rs1
require(p->extension_enabled('V') || P.VU.vsew < e64);

VI_VX_LOOP
({
  vd = ((int128_t)vs2 * rs1) >> sew;
})
