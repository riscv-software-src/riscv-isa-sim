// See LICENSE for license details.

#include "insn_template.h"

reg_t rv32_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  int xlen = 32;
  reg_t npc = sext_xlen(pc + insn_length(OPCODE));
  const uint32_t local_op = OPCODE;
  #include "insns/NAME.h"
  VI_CHECK_IMPL;
  trace_opcode(p, OPCODE, insn);
  return npc;
}

reg_t rv64_NAME(processor_t* p, insn_t insn, reg_t pc)
{
  int xlen = 64;
  reg_t npc = sext_xlen(pc + insn_length(OPCODE));
  const uint32_t local_op = OPCODE;
  #include "insns/NAME.h"
  VI_CHECK_IMPL;
  trace_opcode(p, OPCODE, insn);
  return npc;
}
