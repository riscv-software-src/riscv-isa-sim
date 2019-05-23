// vssra.vx vd, vs2, rs1
VRM xrm = p->VU.get_vround_mode();
VI_VX_LOOP
({
  uint64_t v2 = vs2;
  INT_ROUNDING(v2, xrm, 1);
  vd = vsext(v2, sew) >> (rs1 & ((2 * sew) - 1));
})
VI_CHECK_1905
