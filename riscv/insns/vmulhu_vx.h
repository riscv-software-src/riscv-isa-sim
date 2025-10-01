// vmulhu vd ,vs2, rs1
if (P.VU.ELEN == e64 && p->get_isa().get_zve())
  require(P.VU.vsew < e64);

VI_VX_ULOOP
({
  vd = ((uint128_t)vs2 * rs1) >> sew;
})
