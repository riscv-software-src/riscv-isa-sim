if(xlen == 32)
  if(((int32_t) (RS1)) < ((int32_t) (RS2)))
    set_pc(BRANCH_TARGET);
else
if(sreg_t(RS1) < sreg_t(RS2))
  set_pc(BRANCH_TARGET);
