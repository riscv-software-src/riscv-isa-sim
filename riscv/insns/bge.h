if(sreg_t(cmp_trunc(RS1)) >= sreg_t(cmp_trunc(RS2)))
  set_pc(BRANCH_TARGET);
