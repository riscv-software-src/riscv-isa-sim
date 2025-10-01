// vmulhsu.vx vd, vs2, rs1
if (P.VU.ELEN == e64 && p->get_isa().get_zve())
  require(P.VU.vsew < e64);

VI_VX_SU_LOOP({
  vd = ((int128_t)vs2 * (uint128_t)rs1) >> sew;
})
