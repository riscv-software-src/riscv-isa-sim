// See LICENSE for license details.

#include "config.h"
#include "processor.h"
#include "mmu.h"
#include "softfloat.h"
#include "platform.h" // softfloat isNaNF32UI, etc.
#include "internals.h" // ditto
#include "hwacha.h"
#include "decode_hwacha_ut_half.h"
#include "cvt16.h"
#include <assert.h>

reg_t hwacha_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  int xprlen = 64;
  reg_t npc = sext_xprlen(pc + insn_length(OPCODE));
  hwacha_t* h = static_cast<hwacha_t*>(p->get_extension());
  do {
    #include "insns_ut_half/NAME.h"
    WRITE_UTIDX(UTIDX+1);
  } while (UTIDX < VL);
  WRITE_UTIDX(0);
  return npc;
}
