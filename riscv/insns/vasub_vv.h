// vasub: Averaging subs of integers 
VRM xrm = STATE.VU.get_vround_mode();
VI_VV_LOOP
({
  int64_t ret = (int64_t)vs2 - vs1;
  INT_ROUNDING(ret, xrm, 1);
  vd = ret >> 1;
})
