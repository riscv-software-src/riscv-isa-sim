// vssra.vx vd, vs2, rs1
VRM xrm = p->VU.get_vround_mode();
VI_VX_LOOP
({
  INT_ROUNDING(vs2, xrm, sew);
  vd = vs2 >> (rs1 & ((1u << log2(sew)) - 1));
})
