// vssra.vx vd, vs2, rs1
VRM xrm = p->VU.get_vround_mode();
VI_VX_LOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, sew);
  vd = vsext(v2 >> (rs1 & ((2 * sew) - 1)), sew);
})
