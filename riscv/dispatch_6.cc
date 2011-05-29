#include "insn_includes.h"

reg_t processor_t::opcode_func_006(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_010(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_01a(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_024(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_02e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_038(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_042(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x2)
  {
    reg_t npc = pc + insn_length(0x00000002);
    #include "insns/c_move.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8002)
  {
    reg_t npc = pc + insn_length(0x00008002);
    #include "insns/c_j.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_04c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_056(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_060(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_06a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_074(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_07e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_088(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_092(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x12)
  {
    reg_t npc = pc + insn_length(0x00000012);
    #include "insns/c_lw0.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8012)
  {
    reg_t npc = pc + insn_length(0x00008012);
    #include "insns/c_ld0.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_09c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001c);
    #include "insns/c_add3.h"
    return npc;
}

reg_t processor_t::opcode_func_0a6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0b0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_0ba(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0c4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0ce(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0d8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_0e2(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x2)
  {
    reg_t npc = pc + insn_length(0x00000002);
    #include "insns/c_move.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8002)
  {
    reg_t npc = pc + insn_length(0x00008002);
    #include "insns/c_j.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0ec(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_0f6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_100(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_10a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_114(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_11e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_128(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_132(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x12)
  {
    reg_t npc = pc + insn_length(0x00000012);
    #include "insns/c_lw0.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8012)
  {
    reg_t npc = pc + insn_length(0x00008012);
    #include "insns/c_ld0.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_13c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_146(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_150(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_15a(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_164(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_16e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_178(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_182(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x2)
  {
    reg_t npc = pc + insn_length(0x00000002);
    #include "insns/c_move.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8002)
  {
    reg_t npc = pc + insn_length(0x00008002);
    #include "insns/c_j.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_18c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_196(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_1a0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_1aa(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_1b4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_1be(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1c8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1d2(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x12)
  {
    reg_t npc = pc + insn_length(0x00000012);
    #include "insns/c_lw0.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8012)
  {
    reg_t npc = pc + insn_length(0x00008012);
    #include "insns/c_ld0.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1dc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_1e6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1f0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_1fa(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_204(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_20e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_218(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_222(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x2)
  {
    reg_t npc = pc + insn_length(0x00000002);
    #include "insns/c_move.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8002)
  {
    reg_t npc = pc + insn_length(0x00008002);
    #include "insns/c_j.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_22c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_236(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_240(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_24a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_254(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_25e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_268(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_272(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x12)
  {
    reg_t npc = pc + insn_length(0x00000012);
    #include "insns/c_lw0.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8012)
  {
    reg_t npc = pc + insn_length(0x00008012);
    #include "insns/c_ld0.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_27c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000021c);
    #include "insns/c_or3.h"
    return npc;
}

reg_t processor_t::opcode_func_286(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_290(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_29a(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2a4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2ae(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2b8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_2c2(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x2)
  {
    reg_t npc = pc + insn_length(0x00000002);
    #include "insns/c_move.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8002)
  {
    reg_t npc = pc + insn_length(0x00008002);
    #include "insns/c_j.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2cc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_2d6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_2e0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_2ea(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_2f4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_2fe(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_308(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_312(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x12)
  {
    reg_t npc = pc + insn_length(0x00000012);
    #include "insns/c_lw0.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8012)
  {
    reg_t npc = pc + insn_length(0x00008012);
    #include "insns/c_ld0.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_31c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

reg_t processor_t::opcode_func_326(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_330(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_33a(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_344(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_34e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_358(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_362(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x2)
  {
    reg_t npc = pc + insn_length(0x00000002);
    #include "insns/c_move.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8002)
  {
    reg_t npc = pc + insn_length(0x00008002);
    #include "insns/c_j.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_36c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_376(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_380(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_38a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_394(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_39e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3a8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3b2(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x12)
  {
    reg_t npc = pc + insn_length(0x00000012);
    #include "insns/c_lw0.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x8012)
  {
    reg_t npc = pc + insn_length(0x00008012);
    #include "insns/c_ld0.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3bc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

reg_t processor_t::opcode_func_3c6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3d0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_3da(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x801f) == 0x801a)
  {
    reg_t npc = pc + insn_length(0x0000801a);
    #include "insns/c_sub.h"
    return npc;
  }
  if((insn.bits & 0x801f) == 0x1a)
  {
    reg_t npc = pc + insn_length(0x0000001a);
    #include "insns/c_add.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3e4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3ee(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3f8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

