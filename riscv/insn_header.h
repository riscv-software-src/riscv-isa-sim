#include "processor.h"
#include "common.h"
#include "config.h"
#include "sim.h"
#include "softfloat.h"
#include "platform.h" // softfloat isNaNF32UI, etc.
#include "internals.h" // ditto

#define DECLARE_INSN(name, opcode, mask) \
  const uint32_t MATCH_ ## name = opcode; \
  const uint32_t MASK_ ## name = mask;
#include "opcodes.h"
#undef DECLARE_INSN

reg_t processor_t::FUNC (insn_t insn, reg_t pc)
{
  #define MASK (~(DISPATCH_TABLE_SIZE-1) & OPCODE_MASK)
  #define MATCH (~(DISPATCH_TABLE_SIZE-1) & OPCODE_MATCH)
  if(unlikely((insn.bits & MASK) != MATCH))
    throw trap_illegal_instruction;

  reg_t npc = pc + insn_length(OPCODE_MATCH);
