require_extension(EXT_ZIBI);

if (RS1 == insn.b_imm5()) {
  set_pc(BRANCH_TARGET);
}
