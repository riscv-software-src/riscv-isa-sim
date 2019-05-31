// vssrl.vx vd, vs2, rs1
VRM xrm = p->VU.get_vround_mode();
VI_VX_ULOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, 1);
  int sh = rs1 & ((1u << log2(sew * 2)) - 1);

  if (sh > 63)
    vd = (v2 >> 63) >> 1;
  else
    vd = v2 >> sh;
})
VI_CHECK_1905
