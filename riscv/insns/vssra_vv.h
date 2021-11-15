// vssra.vv vd, vs2, vs1
VRM xrm = p->VU.get_vround_mode();
VI_VV_LOOP
({
  int sh = vs1 & (sew - 1);
  int128_t val = vs2;

  INT_ROUNDING(val, xrm, sh);
  vd = val >> sh;
})
