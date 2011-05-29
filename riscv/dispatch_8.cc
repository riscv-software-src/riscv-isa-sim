#include "insn_includes.h"

reg_t processor_t::opcode_func_008(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_012(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_01c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001c);
    #include "insns/c_add3.h"
    return npc;
}

reg_t processor_t::opcode_func_026(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_030(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_03a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_044(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_04e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_058(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_062(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_06c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_076(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_080(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_08a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_094(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_09e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0a8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0b2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0bc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001c);
    #include "insns/c_add3.h"
    return npc;
}

reg_t processor_t::opcode_func_0c6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0d0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_0da(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0e4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0ee(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0f8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_102(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_10c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_116(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_120(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_12a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_134(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_13e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_148(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_152(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_15c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_166(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_170(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_17a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_184(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_18e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_198(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_1a2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1ac(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_1b6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_1c0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_1ca(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_1d4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_1de(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1e8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1f2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1fc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_206(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_210(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_21a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_224(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_22e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_238(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_242(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_24c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_256(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_260(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_26a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_274(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_27e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_288(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_292(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_29c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000021c);
    #include "insns/c_or3.h"
    return npc;
}

reg_t processor_t::opcode_func_2a6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2b0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_2ba(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2c4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2ce(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2d8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_2e2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2ec(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_2f6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_300(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_30a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_314(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_31e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_328(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_332(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_33c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

reg_t processor_t::opcode_func_346(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_350(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_35a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_364(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_36e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_378(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_382(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_38c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_396(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_3a0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_3aa(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_3b4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_3be(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3c8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3d2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3dc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

reg_t processor_t::opcode_func_3e6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3f0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_3fa(insn_t insn, reg_t pc)
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

