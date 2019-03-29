// vfmerge_vf vd, vs2, vs1, vm
VFP_VF_LOOP
({
  bool do_mask = false;

  V_CHECK_MASK(do_mask);

  bool use_first = (insn.v_vm() == 1) || do_mask;
  vd = use_first ? vs2 : rs1;
})
