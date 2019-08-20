// vssrl.vv vd, vs2, vs1
VRM xrm = P.VU.get_vround_mode();
VI_VV_ULOOP
({
  int sh = vs1 & (sew - 1);

  INT_ROUNDING(vs2, xrm, sh);
  vd = vs2 >> sh;
})
