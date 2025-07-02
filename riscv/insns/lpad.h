if (ZICFILP_xLPE(STATE.v, STATE.prv) && STATE.elp == elp_t::LP_EXPECTED) {
  software_check(pc % 4 == 0 &&
                 ((READ_REG(7) & 0xFFFFF000ULL) == (static_cast<uint64_t>(insn.u_imm()) & 0xFFFFF000ULL) || insn.u_imm() == 0LL),
                 LANDING_PAD_FAULT);
  STATE.elp = elp_t::NO_LP_EXPECTED;
}
