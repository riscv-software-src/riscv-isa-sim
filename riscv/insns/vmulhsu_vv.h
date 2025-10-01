// vmulhsu.vv vd, vs2, vs1
if (P.VU.ELEN == e64 && p->get_isa().get_zve())
  require(P.VU.vsew < e64);

VI_VV_SU_LOOP({
  vd = ((int128_t)vs2 * (uint128_t)vs1) >> sew;
})
