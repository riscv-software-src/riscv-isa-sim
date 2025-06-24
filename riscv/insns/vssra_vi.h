// vssra.vi vd, vs2, zimm5
VI_VI_LOOP
({
  VRM xrm = P.VU.get_vround_mode();
  int sh = insn.v_zimm5() & (sew - 1);
  int128_t val = vs2;

  INT_ROUNDING(val, xrm, sh);
  vd = val >> sh;
})
