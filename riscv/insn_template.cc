// See LICENSE for license details.

#include "insn_template.h"
#include "insn_macros.h"

reg_t rv32_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  #define xlen 32
  reg_t npc = sext_xlen(pc + insn_length(OPCODE));
  #include "insns/NAME.h"
  trace_opcode(p, OPCODE, insn);
  #undef xlen
  return npc;
}

reg_t rv64_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  #define xlen 64
  reg_t npc = sext_xlen(pc + insn_length(OPCODE));
  #include "insns/NAME.h"
  trace_opcode(p, OPCODE, insn);
  #undef xlen
  return npc;
}
