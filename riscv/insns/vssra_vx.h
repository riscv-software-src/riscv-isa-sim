// vssra.vx vd, vs2, rs1
VRM xrm = P.VU.get_vround_mode();
VI_VX_LOOP
({
  int sh = rs1 & (sew - 1);
  int128_t val = vs2;

  INT_ROUNDING(val, xrm, sh);
  vd = val >> sh;
})
