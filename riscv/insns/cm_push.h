require_zcmp_pushpop;
if (xlen == 32)
  MMU.zcmp_push<int32_t>(insn.zcmp_regmask(), insn.zcmp_stack_adjustment(xlen));
else
  MMU.zcmp_push<int64_t>(insn.zcmp_regmask(), insn.zcmp_stack_adjustment(xlen));
