#include "insn_includes.h"

reg_t processor_t::opcode_func_004(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_00e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_018(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_022(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_02c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_036(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_040(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_04a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_054(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_05e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_068(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_072(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_07c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001c);
    #include "insns/c_add3.h"
    return npc;
}

reg_t processor_t::opcode_func_086(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_090(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_09a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0a4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0ae(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0b8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_0c2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0cc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_0d6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_0e0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_0ea(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_0f4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_0fe(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_108(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_112(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_11c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_126(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_130(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_13a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_144(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_14e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_158(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_162(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_16c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_176(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_180(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_18a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_194(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_19e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1a8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1b2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1bc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000011c);
    #include "insns/c_sub3.h"
    return npc;
}

reg_t processor_t::opcode_func_1c6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1d0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_1da(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1e4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1ee(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1f8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_202(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_20c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_216(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_220(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_22a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_234(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_23e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_248(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_252(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_25c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000021c);
    #include "insns/c_or3.h"
    return npc;
}

reg_t processor_t::opcode_func_266(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_270(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_27a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_284(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_28e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_298(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_2a2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2ac(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_2b6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_2c0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_2ca(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_2d4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_2de(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2e8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2f2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2fc(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000021c);
    #include "insns/c_or3.h"
    return npc;
}

reg_t processor_t::opcode_func_306(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_310(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_31a(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_324(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_32e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_338(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_342(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_34c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_356(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_360(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000000);
    #include "insns/c_li.h"
    return npc;
}

reg_t processor_t::opcode_func_36a(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000a);
    #include "insns/c_lw.h"
    return npc;
}

reg_t processor_t::opcode_func_374(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000014);
    #include "insns/c_flw.h"
    return npc;
}

reg_t processor_t::opcode_func_37e(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_388(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000008);
    #include "insns/c_swsp.h"
    return npc;
}

reg_t processor_t::opcode_func_392(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_39c(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000031c);
    #include "insns/c_and3.h"
    return npc;
}

reg_t processor_t::opcode_func_3a6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000006);
    #include "insns/c_sdsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3b0(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000010);
    #include "insns/c_beq.h"
    return npc;
}

reg_t processor_t::opcode_func_3ba(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3c4(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000004);
    #include "insns/c_ldsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3ce(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3d8(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000018);
    #include "insns/c_fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_3e2(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3ec(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000c);
    #include "insns/c_sd.h"
    return npc;
}

reg_t processor_t::opcode_func_3f6(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000016);
    #include "insns/c_fsw.h"
    return npc;
}

