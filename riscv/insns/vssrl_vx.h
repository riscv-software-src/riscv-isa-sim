// vssrl.vx vd, vs2, rs1
VRM xrm = p->VU.get_vround_mode();
VI_VX_ULOOP
({
  INT_ROUNDING(vs2, xrm, sew);
  vd = vs2 >> (rs1 & ((1u << log2(sew)) - 1));
})
