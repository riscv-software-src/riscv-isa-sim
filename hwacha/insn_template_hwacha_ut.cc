// See LICENSE for license details.
#include "insn_template_hwacha_ut.h"

reg_t hwacha_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  int xprlen = 64;
  reg_t npc = sext_xprlen(pc + insn_length(OPCODE));
  hwacha_t* h = static_cast<hwacha_t*>(p->get_extension());
  do {
    #include "insns_ut/NAME.h"
    WRITE_UTIDX(UTIDX+1);
  } while (UTIDX < VL);
  WRITE_UTIDX(0);
  return npc;
}
