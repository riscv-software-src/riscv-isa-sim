#include "insn_includes.h"

reg_t processor_t::opcode_func_005(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_00f(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xfff) == 0x80f)
  {
    reg_t npc = pc + insn_length(0x0000080f);
    #include "insns/vssegstb.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0xf)
  {
    reg_t npc = pc + insn_length(0x0000000f);
    #include "insns/vsb.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x100f)
  {
    reg_t npc = pc + insn_length(0x0000100f);
    #include "insns/vsstb.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x200f)
  {
    reg_t npc = pc + insn_length(0x0000200f);
    #include "insns/vssegb.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_019(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_023(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000023);
    #include "insns/sb.h"
    return npc;
}

reg_t processor_t::opcode_func_02d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_037(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_041(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_04b(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000004b);
    #include "insns/fnmsub_s.h"
    return npc;
}

reg_t processor_t::opcode_func_055(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_05f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_069(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_073(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x1873)
  {
    reg_t npc = pc + insn_length(0x00001873);
    #include "insns/vmts.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x73)
  {
    reg_t npc = pc + insn_length(0x00000073);
    #include "insns/vmvv.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x873)
  {
    reg_t npc = pc + insn_length(0x00000873);
    #include "insns/vmsv.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x1073)
  {
    reg_t npc = pc + insn_length(0x00001073);
    #include "insns/vmst.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_07d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_087(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_091(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_09b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3f83ff) == 0x9b)
  {
    reg_t npc = pc + insn_length(0x0000009b);
    #include "insns/slliw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0a5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0af(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000af);
    #include "insns/fence_i.h"
    return npc;
}

reg_t processor_t::opcode_func_0b9(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0c3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000c3);
    #include "insns/fmadd_d.h"
    return npc;
}

reg_t processor_t::opcode_func_0cd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_0d7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0e1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_0eb(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000eb);
    #include "insns/jalr_r.h"
    return npc;
}

reg_t processor_t::opcode_func_0f5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_0ff(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_109(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_113(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000113);
    #include "insns/slti.h"
    return npc;
}

reg_t processor_t::opcode_func_11d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_127(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000127);
    #include "insns/fsw.h"
    return npc;
}

reg_t processor_t::opcode_func_131(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_13b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_145(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_14f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_159(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_163(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_16d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_177(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xffffffff) == 0x177)
  {
    reg_t npc = pc + insn_length(0x00000177);
    #include "insns/stop.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_181(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_18b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x158b)
  {
    reg_t npc = pc + insn_length(0x0000158b);
    #include "insns/vflstd.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x18b)
  {
    reg_t npc = pc + insn_length(0x0000018b);
    #include "insns/vld.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x118b)
  {
    reg_t npc = pc + insn_length(0x0000118b);
    #include "insns/vlstd.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0x98b)
  {
    reg_t npc = pc + insn_length(0x0000098b);
    #include "insns/vlsegstd.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x258b)
  {
    reg_t npc = pc + insn_length(0x0000258b);
    #include "insns/vflsegd.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0xd8b)
  {
    reg_t npc = pc + insn_length(0x00000d8b);
    #include "insns/vflsegstd.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x58b)
  {
    reg_t npc = pc + insn_length(0x0000058b);
    #include "insns/vfld.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x218b)
  {
    reg_t npc = pc + insn_length(0x0000218b);
    #include "insns/vlsegd.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_195(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_19f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1a9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_1b3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x1b3)
  {
    reg_t npc = pc + insn_length(0x000001b3);
    #include "insns/sltu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x5b3)
  {
    reg_t npc = pc + insn_length(0x000005b3);
    #include "insns/mulhu.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1bd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_1c7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1d1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_1db(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1e5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1ef(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_1f9(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_203(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000203);
    #include "insns/lbu.h"
    return npc;
}

reg_t processor_t::opcode_func_20d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_217(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_221(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_22b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_235(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_23f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_249(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_253(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3ff1ff) == 0x9053)
  {
    reg_t npc = pc + insn_length(0x00009053);
    #include "insns/fcvt_lu_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x11053)
  {
    reg_t npc = pc + insn_length(0x00011053);
    #include "insns/fcvt_s_d.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xe053)
  {
    reg_t npc = pc + insn_length(0x0000e053);
    #include "insns/fcvt_s_w.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x8053)
  {
    reg_t npc = pc + insn_length(0x00008053);
    #include "insns/fcvt_l_s.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x3053)
  {
    reg_t npc = pc + insn_length(0x00003053);
    #include "insns/fdiv_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xd053)
  {
    reg_t npc = pc + insn_length(0x0000d053);
    #include "insns/fcvt_s_lu.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x2053)
  {
    reg_t npc = pc + insn_length(0x00002053);
    #include "insns/fmul_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xb053)
  {
    reg_t npc = pc + insn_length(0x0000b053);
    #include "insns/fcvt_wu_s.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x1053)
  {
    reg_t npc = pc + insn_length(0x00001053);
    #include "insns/fsub_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xf053)
  {
    reg_t npc = pc + insn_length(0x0000f053);
    #include "insns/fcvt_s_wu.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xc053)
  {
    reg_t npc = pc + insn_length(0x0000c053);
    #include "insns/fcvt_s_l.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xa053)
  {
    reg_t npc = pc + insn_length(0x0000a053);
    #include "insns/fcvt_w_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x4053)
  {
    reg_t npc = pc + insn_length(0x00004053);
    #include "insns/fsqrt_s.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x53)
  {
    reg_t npc = pc + insn_length(0x00000053);
    #include "insns/fadd_s.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_25d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_267(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_271(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_27b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xffffffff) == 0x27b)
  {
    reg_t npc = pc + insn_length(0x0000027b);
    #include "insns/eret.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_285(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_28f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_299(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2a3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2ad(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_2b7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_2c1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_2cb(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000cb);
    #include "insns/fnmsub_d.h"
    return npc;
}

reg_t processor_t::opcode_func_2d5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_2df(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2e9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_2f3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3fffff) == 0x2f3)
  {
    reg_t npc = pc + insn_length(0x000002f3);
    #include "insns/vsetvl.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2fd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_307(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_311(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_31b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_325(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_32f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000032f);
    #include "insns/fence_l_cv.h"
    return npc;
}

reg_t processor_t::opcode_func_339(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_343(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_34d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_357(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_361(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_36b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_375(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_37f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_389(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_393(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000393);
    #include "insns/andi.h"
    return npc;
}

reg_t processor_t::opcode_func_39d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_3a7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3b1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_3bb(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x7bb)
  {
    reg_t npc = pc + insn_length(0x000007bb);
    #include "insns/remuw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3c5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3cf(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3d9(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1c1f) == 0x19)
  {
    reg_t npc = pc + insn_length(0x00000019);
    #include "insns/c_slli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x819)
  {
    reg_t npc = pc + insn_length(0x00000819);
    #include "insns/c_srli.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1419)
  {
    reg_t npc = pc + insn_length(0x00001419);
    #include "insns/c_srai32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1819)
  {
    reg_t npc = pc + insn_length(0x00001819);
    #include "insns/c_slliw.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0xc19)
  {
    reg_t npc = pc + insn_length(0x00000c19);
    #include "insns/c_srli32.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x1019)
  {
    reg_t npc = pc + insn_length(0x00001019);
    #include "insns/c_srai.h"
    return npc;
  }
  if((insn.bits & 0x1c1f) == 0x419)
  {
    reg_t npc = pc + insn_length(0x00000419);
    #include "insns/c_slli32.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3e3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000003e3);
    #include "insns/bgeu.h"
    return npc;
}

reg_t processor_t::opcode_func_3ed(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_3f7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

