// vssrl.vv vd, vs2, vs1
VRM xrm = p->VU.get_vround_mode();
VI_VV_ULOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, 1);
  vd = vzext(v2 >> (vs1 & (2 * sew - 1)), sew);
})
VI_CHECK_1905
