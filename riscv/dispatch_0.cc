#include "insn_includes.h"

reg_t processor_t::opcode_func_000(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_00a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_014(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_01e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_028(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_032(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_03c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001c);
    #include "insns/c_add3.h"
    return npc;
}

reg_t processor_t::opcode_func_046(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_050(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_05a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_064(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_06e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_078(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_082(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_08c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_096(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_0a0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_0aa(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_0b4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_0be(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0c8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0d2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0dc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001c);
    #include "insns/c_add3.h"
    return npc;
}

reg_t processor_t::opcode_func_0e6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0f0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_0fa(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_104(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_10e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_118(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_122(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_12c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_136(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_140(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_14a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_154(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_15e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_168(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_172(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_17c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_186(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_190(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_19a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1a4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1ae(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1b8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_1c2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1cc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_1d6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_1e0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_1ea(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_1f4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_1fe(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_208(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_212(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_21c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000021c);
    #include "insns/c_or3.h"
    return npc;
}

reg_t processor_t::opcode_func_226(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_230(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_23a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_244(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_24e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_258(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_262(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_26c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_276(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_280(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_28a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_294(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_29e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2a8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2b2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2bc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000021c);
    #include "insns/c_or3.h"
    return npc;
}

reg_t processor_t::opcode_func_2c6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2d0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_2da(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2e4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2ee(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2f8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_302(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_30c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_316(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_320(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_32a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_334(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_33e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_348(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_352(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_35c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

reg_t processor_t::opcode_func_366(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_370(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_37a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_384(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_38e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_398(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_3a2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3ac(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_3b6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_3c0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_3ca(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_3d4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_3de(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3e8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3f2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3fc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

