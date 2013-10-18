if (VL) {
  if (!h->vf_active()) {
    WRITE_VF_PC(XS1 + insn.s_imm());
    for (uint32_t i=0; i<VL; i++)
      h->get_ut_state(i)->run = true;
  }

  if (VF_PC & 3)
    h->take_exception(HWACHA_CAUSE_VF_MISALIGNED_FETCH, VF_PC);

  mmu_t::insn_fetch_t ut_fetch = p->get_mmu()->load_insn(VF_PC);
  insn_t ut_insn = ut_fetch.insn.insn;

  bool matched = false;

  #define DECLARE_INSN(name, match, mask) \
    extern reg_t hwacha_##name(processor_t*, insn_t, reg_t); \
    if ((ut_insn.bits() & mask) == match) { \
      WRITE_VF_PC(hwacha_##name(p, ut_insn, VF_PC)); \
      matched = true; \
    }
  #include "opcodes_hwacha_ut.h"
  #undef DECLARE_INSN

  if (!matched)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_INSTRUCTION, VF_PC);

  // if vf is still running, rewind pc so that it will execute again
  if (h->vf_active())
    npc = pc;
}
