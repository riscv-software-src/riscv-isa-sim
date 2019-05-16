// vssra.vv vd, vs2, vs1
VRM xrm = p->VU.get_vround_mode();
VI_VV_LOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, 1);
  vd = vsext(v2 >> (vs1 & ((2 * sew) - 1)), sew);
})
