// vmulhsu.vx vd, vs2, rs1
require(p->extension_enabled('V') || P.VU.vsew < e64);

VI_VX_SU_LOOP({
  if (xlen == 64)
    vd = ((int128_t)(int64_t)vs2 * (uint64_t)rs1) >> sew;
  else
    vd = ((int128_t)(int64_t)vs2 * (uint32_t)rs1) >> sew;
})
