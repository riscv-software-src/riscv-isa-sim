// vmulh vd, vs2, vs1
if (P.VU.ELEN == e64 && p->get_isa().get_zve())
  require(P.VU.vsew < e64);

VI_VV_LOOP
({
  vd = ((int128_t)vs2 * vs1) >> sew;
})
