// vssra.vi vd, vs2, simm5
VRM xrm = p->VU.get_vround_mode();
VI_VI_LOOP
({
  INT_ROUNDING(vs2, xrm, sew);
  vd = vs2 >> ((uint8_t)simm5 & ((1u << log2(sew)) - 1));
})
