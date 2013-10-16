#include "hwacha.h"

void ct_state_t::reset()
{
  vl = 0;
  maxvl = 32;
  nxpr = 32;
  nfpr = 32;

  vf_pc = -1;
}

void ut_state_t::reset()
{
  run = false;
  XPR.reset();
  FPR.reset();
}

hwacha_t::hwacha_t()
{
  ct_state.reset();
  for (int i=0; i<max_uts; i++)
    ut_state[i].reset();
}

std::vector<insn_desc_t> hwacha_t::get_instructions()
{
  std::vector<insn_desc_t> insns;
  #define DECLARE_INSN(name, match, mask) \
    extern reg_t hwacha_##name(processor_t*, insn_t, reg_t); \
    insns.push_back((insn_desc_t){match, mask, &::illegal_instruction, hwacha_##name});
  #include "opcodes_hwacha.h"
  #undef DECLARE_INSN
  return insns;
}

bool hwacha_t::vf_active()
{
  for (uint32_t i=0; i<get_ct_state()->vl; i++) {
    if (get_ut_state(i)->run)
      return true;
  }

  return false;
}
