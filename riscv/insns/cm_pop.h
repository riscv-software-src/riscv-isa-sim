require_zcmp_pushpop;
if (xlen == 32)
  MMU.zcmp_pop<int32_t>(insn.zcmp_regmask(), insn.zcmp_stack_adjustment(xlen));
else
  MMU.zcmp_pop<int64_t>(insn.zcmp_regmask(), insn.zcmp_stack_adjustment(xlen));
