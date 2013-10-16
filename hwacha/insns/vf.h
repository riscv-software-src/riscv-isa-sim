if (VL) {
  if (!h->vf_active()) {
    h->get_ct_state()->vf_pc = XS1 + insn.s_imm();
    for (uint32_t i=0; i<VL; i++)
      h->get_ut_state(i)->run = true;
  }

  mmu_t::insn_fetch_t ut_fetch = p->get_mmu()->load_insn(h->get_ct_state()->vf_pc);
  insn_t ut_insn = ut_fetch.insn.insn;

  bool matched = false;

  #define DECLARE_INSN(name, match, mask) \
    extern reg_t hwacha_##name(processor_t*, insn_t, reg_t); \
    if ((ut_insn.bits() & mask) == match) { \
      h->get_ct_state()->vf_pc = hwacha_##name(p, ut_insn, h->get_ct_state()->vf_pc); \
      matched = true; \
    }
  #include "opcodes_hwacha_ut.h"
  #undef DECLARE_INSN

  // YUNSUP FIXME
  assert(matched);

  // if vf is still running, rewind pc so that it will execute again
  if (h->vf_active())
    npc = pc;
}
