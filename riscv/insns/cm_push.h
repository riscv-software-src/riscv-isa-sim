require_zcmp_pushpop;

const auto new_sp = SP - insn.zcmp_stack_adjustment(xlen);
auto addr = SP;

for (int i = Sn(11); i >= 0; i--) {
  if (insn.zcmp_regmask() & (1 << i)) {
    addr -= xlen / 8;

    if (xlen == 32)
      MMU.store<uint32_t>(addr, READ_REG(i));
    else
      MMU.store<uint64_t>(addr, READ_REG(i));
  }
}

WRITE_REG(X_SP, new_sp);
