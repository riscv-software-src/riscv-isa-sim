// vmerge.vv vd, vs2, vs1, vm
VI_VV_LOOP
({
  bool do_mask = false;

  if (insn.v_vm() == 0) {
    int midx = (STATE.VU.vmlen * i) / 32;
    int mpos = (STATE.VU.vmlen * i) % 32;
    do_mask = (STATE.VU.elt<uint32_t>(0, midx) >> mpos) & 0x1;
  }

  bool use_first = (insn.v_vm() == 1) || do_mask;
  switch (STATE.VU.vsew) {
  case e8:
    STATE.VU.elt<uint8_t>(rd_num, i) = use_first ? vs1 : vs2;
    break;
  case e16:
    STATE.VU.elt<uint16_t>(rd_num, i) = use_first ? vs1 : vs2;
    break;
  case e32:
    STATE.VU.elt<uint32_t>(rd_num, i) = use_first ? vs1 : vs2;
    break;
  default:
    STATE.VU.elt<uint64_t>(rd_num, i) = use_first ? vs1 : vs2;
  }
 })
