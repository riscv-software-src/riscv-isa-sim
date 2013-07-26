// See LICENSE for license details.

#include "processor.h"
#include "config.h"
#include "sim.h"
#include "softfloat.h"
#include "platform.h" // softfloat isNaNF32UI, etc.
#include "internals.h" // ditto
#include <assert.h>

reg_t processor_t::rv32_NAME(insn_t insn, reg_t pc)
{
  int xprlen = 32;
  reg_t npc = sext_xprlen(pc + insn_length(OPCODE));
  #include "insns/NAME.h"
  return npc;
}

reg_t processor_t::rv64_NAME(insn_t insn, reg_t pc)
{
  int xprlen = 64;
  reg_t npc = sext_xprlen(pc + insn_length(OPCODE));
  #include "insns/NAME.h"
  return npc;
}
