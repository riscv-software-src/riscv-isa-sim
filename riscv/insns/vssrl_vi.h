// vssra.vi vd, vs2, zimm5
VI_VI_ULOOP
({
  VRM xrm = P.VU.get_vround_mode();
  int sh = zimm5 & (sew - 1);
  uint128_t val = vs2;

  INT_ROUNDING(val, xrm, sh);
  vd = val >> sh;
})
