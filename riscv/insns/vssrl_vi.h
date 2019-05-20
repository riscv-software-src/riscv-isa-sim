// vssra.vi vd, vs2, simm5
VRM xrm = p->VU.get_vround_mode();
VI_VI_ULOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, sew);
  vd = v2 >> (simm5 & 0x1f);
})
VI_CHECK_1905
