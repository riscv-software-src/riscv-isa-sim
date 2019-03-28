// vmpopc rd, vs2, vm
require(STATE.VU.vstart == 0);
reg_t popcount = 0;
VI_LOOP_BASE
  bool has_count = STATE.VU.elt<int8_t>(rd_num,  (STATE.VU.vsew / 8) * i) & 0x1;
  bool do_mask = false;
  V_CHECK_MASK(do_mask);

  if ((do_mask && has_count) || has_count)
    popcount++;

VI_LOOP_END
WRITE_RD(popcount);

