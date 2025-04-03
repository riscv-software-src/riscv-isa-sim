// vssrl.vx vd, vs2, rs1
VI_VX_ULOOP
({
  VRM xrm = P.VU.get_vround_mode();
  int sh = rs1 & (sew - 1);
  uint128_t val = vs2;

  INT_ROUNDING(val, xrm, sh);
  vd = val >> sh;
})
