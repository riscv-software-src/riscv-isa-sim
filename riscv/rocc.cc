// See LICENSE for license details.

#include "rocc.h"
#include "trap.h"
#include <cstdlib>

#define customX(n) \
  static reg_t c##n(processor_t* p, insn_t insn, reg_t pc) \
  { \
    rocc_t* rocc = static_cast<rocc_t*>(p->get_extension()); \
    rocc_insn_union_t u; \
    u.i = insn; \
    reg_t xs1 = u.r.xs1 ? RS1 : -1; \
    reg_t xs2 = u.r.xs2 ? RS2 : -1; \
    reg_t xd = rocc->custom##n(u.r, xs1, xs2); \
    if (u.r.xd) \
      WRITE_RD(xd); \
    return pc+4; \
  } \
  \
  reg_t rocc_t::custom##n(rocc_insn_t insn, reg_t xs1, reg_t xs2) \
  { \
    illegal_instruction(); \
    return 0; \
  }

customX(0)
customX(1)
customX(2)
customX(3)

std::vector<insn_desc_t> rocc_t::get_instructions()
{
  std::vector<insn_desc_t> insns;
  insns.push_back((insn_desc_t){0x0b, 0x7f, &::illegal_instruction, c0});
  insns.push_back((insn_desc_t){0x2b, 0x7f, &::illegal_instruction, c1});
  insns.push_back((insn_desc_t){0x5b, 0x7f, &::illegal_instruction, c2});
  insns.push_back((insn_desc_t){0x7b, 0x7f, &::illegal_instruction, c3});
  return insns;
}

std::vector<disasm_insn_t*> rocc_t::get_disasms()
{
  std::vector<disasm_insn_t*> insns;
  return insns;
}
