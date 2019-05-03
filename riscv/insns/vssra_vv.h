// vssra.vv vd, vs2, vs1
VRM xrm = p->VU.get_vround_mode();
VI_VV_LOOP
({
  INT_ROUNDING(vs2, xrm, sew);
  vd = vs2 >> (vs1 & ((1u << log2(sew)) - 1));
})
