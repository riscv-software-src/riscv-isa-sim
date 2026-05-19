// vmulhu vd ,vs2, rs1
require(p->extension_enabled('V') || P.VU.vsew < e64);

VI_VX_ULOOP
({
  if (xlen == 64)
    vd = ((uint128_t)vs2 * (uint64_t)rs1) >> sew;
  else
    vd = ((uint128_t)vs2 * (uint32_t)rs1) >> sew;
})
