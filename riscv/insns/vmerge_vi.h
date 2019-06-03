// vmerge.vi vd, vs2, simm5, vm
bool do_mask = false;
bool use_first = false;

VI_VVXI_MERGE_LOOP
({
  if (insn.v_vm() == 0) {
    int midx = (P.VU.vmlen * i) / 32;
    int mpos = (P.VU.vmlen * i) % 32;
    do_mask = (P.VU.elt<uint32_t>(0, midx) >> mpos) & 0x1;
  }
  
  use_first = (insn.v_vm() == 1) || do_mask;
  vd = use_first ? vsext(simm5, sew) : vs2;
})
