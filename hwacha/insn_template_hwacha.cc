// See LICENSE for license details.
#include "insn_template_hwacha.h"

reg_t hwacha_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  int xprlen = 64;
  reg_t npc = sext_xprlen(pc + insn_length(OPCODE));
  hwacha_t* h = static_cast<hwacha_t*>(p->get_extension());
  rocc_insn_union_t u;
  u.i = insn;
  reg_t xs1 = u.r.xs1 ? RS1 : -1;
  reg_t xs2 = u.r.xs2 ? RS2 : -1;
  reg_t xd = -1;
  #include "insns/NAME.h"
  if (u.r.xd) WRITE_RD(xd);
  return npc;
}
