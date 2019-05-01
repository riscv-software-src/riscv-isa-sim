// vasub: Averaging subs of integers 
VRM xrm = p->VU.get_vround_mode();
VI_VX_LOOP
({
  int64_t ret = (int64_t)vs2 - rs1;
  INT_ROUNDING(ret, xrm, 1);
  vd = ret >> 1;
})
