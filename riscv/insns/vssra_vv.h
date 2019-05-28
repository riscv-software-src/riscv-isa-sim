// vssra.vv vd, vs2, vs1
VRM xrm = p->VU.get_vround_mode();
VI_VV_LOOP
({
  int64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, 1);
  vd = v2 >> (vs1 & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
