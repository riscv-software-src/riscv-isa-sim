// vssrl.vx vd, vs2, rs1
VRM xrm = p->VU.get_vround_mode();
VI_VX_ULOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, 1);
  vd = v2 >> (rs1 & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
