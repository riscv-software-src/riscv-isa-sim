// vssra.vi vd, vs2, simm5
VRM xrm = P.VU.get_vround_mode();
VI_VI_ULOOP
({
  int sh = simm5 & (sew - 1) & 0x1f;

  INT_ROUNDING(vs2, xrm, sh);
  vd = vs2 >> sh;
})
