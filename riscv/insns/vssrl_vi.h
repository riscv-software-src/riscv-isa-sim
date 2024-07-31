// vssra.vi vd, vs2, simm5
VI_VI_ULOOP
({
  VRM xrm = P.VU.get_vround_mode();
  int sh = zimm5 & (sew - 1) & 0x1f;
  uint128_t val = vs2;

  INT_ROUNDING(val, xrm, sh);
  vd = val >> sh;
})
