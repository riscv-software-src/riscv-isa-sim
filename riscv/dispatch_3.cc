#include "insn_includes.h"

reg_t processor_t::opcode_func_003(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000003);
    #include "insns/lb.h"
    return npc;
}

reg_t processor_t::opcode_func_00d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_017(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_021(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_02b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_035(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_03f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_049(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_053(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3ff1ff) == 0x9053)
  {
    reg_t npc = pc + insn_length(0x00009053);
    #include "insns/fcvt_lu_s.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x18053)
  {
    reg_t npc = pc + insn_length(0x00018053);
    #include "insns/fmin_s.h"
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
  if((insn.bits & 0x7c1ffff) == 0x1c053)
  {
    reg_t npc = pc + insn_length(0x0001c053);
    #include "insns/mftx_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x8053)
  {
    reg_t npc = pc + insn_length(0x00008053);
    #include "insns/fcvt_l_s.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x17053)
  {
    reg_t npc = pc + insn_length(0x00017053);
    #include "insns/fle_s.h"
    return npc;
  }
  if((insn.bits & 0x7ffffff) == 0x1d053)
  {
    reg_t npc = pc + insn_length(0x0001d053);
    #include "insns/mffsr.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x3053)
  {
    reg_t npc = pc + insn_length(0x00003053);
    #include "insns/fdiv_s.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x1f053)
  {
    reg_t npc = pc + insn_length(0x0001f053);
    #include "insns/mtfsr.h"
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
  if((insn.bits & 0x1ffff) == 0x16053)
  {
    reg_t npc = pc + insn_length(0x00016053);
    #include "insns/flt_s.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x15053)
  {
    reg_t npc = pc + insn_length(0x00015053);
    #include "insns/feq_s.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x7053)
  {
    reg_t npc = pc + insn_length(0x00007053);
    #include "insns/fsgnjx_s.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x19053)
  {
    reg_t npc = pc + insn_length(0x00019053);
    #include "insns/fmax_s.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xb053)
  {
    reg_t npc = pc + insn_length(0x0000b053);
    #include "insns/fcvt_wu_s.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x1e053)
  {
    reg_t npc = pc + insn_length(0x0001e053);
    #include "insns/mxtf_s.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x1053)
  {
    reg_t npc = pc + insn_length(0x00001053);
    #include "insns/fsub_s.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x5053)
  {
    reg_t npc = pc + insn_length(0x00005053);
    #include "insns/fsgnj_s.h"
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
  if((insn.bits & 0x1ffff) == 0x6053)
  {
    reg_t npc = pc + insn_length(0x00006053);
    #include "insns/fsgnjn_s.h"
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

reg_t processor_t::opcode_func_05d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_067(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_071(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_07b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x7ffffff) == 0x7b)
  {
    reg_t npc = pc + insn_length(0x0000007b);
    #include "insns/ei.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_085(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_08f(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xfff) == 0x88f)
  {
    reg_t npc = pc + insn_length(0x0000088f);
    #include "insns/vssegsth.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x108f)
  {
    reg_t npc = pc + insn_length(0x0000108f);
    #include "insns/vssth.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x8f)
  {
    reg_t npc = pc + insn_length(0x0000008f);
    #include "insns/vsh.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x208f)
  {
    reg_t npc = pc + insn_length(0x0000208f);
    #include "insns/vssegh.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_099(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0a3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000a3);
    #include "insns/sh.h"
    return npc;
}

reg_t processor_t::opcode_func_0ad(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_0b7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_0c1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_0cb(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000cb);
    #include "insns/fnmsub_d.h"
    return npc;
}

reg_t processor_t::opcode_func_0d5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_0df(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0e9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_0f3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000f3);
    #include "insns/vvcfgivl.h"
    return npc;
}

reg_t processor_t::opcode_func_0fd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_107(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000107);
    #include "insns/flw.h"
    return npc;
}

reg_t processor_t::opcode_func_111(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_11b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_125(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_12f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000012f);
    #include "insns/fence.h"
    return npc;
}

reg_t processor_t::opcode_func_139(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_143(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_14d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_157(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_161(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_16b(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000016b);
    #include "insns/jalr_j.h"
    return npc;
}

reg_t processor_t::opcode_func_175(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_17f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_189(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_193(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000193);
    #include "insns/sltiu.h"
    return npc;
}

reg_t processor_t::opcode_func_19d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_1a7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000001a7);
    #include "insns/fsd.h"
    return npc;
}

reg_t processor_t::opcode_func_1b1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_1bb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1c5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1cf(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1d9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1e3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1ed(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_1f7(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x7ffffff) == 0x1f7)
  {
    reg_t npc = pc + insn_length(0x000001f7);
    #include "insns/utidx.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_201(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_20b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x120b)
  {
    reg_t npc = pc + insn_length(0x0000120b);
    #include "insns/vlstbu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x220b)
  {
    reg_t npc = pc + insn_length(0x0000220b);
    #include "insns/vlsegbu.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0xa0b)
  {
    reg_t npc = pc + insn_length(0x00000a0b);
    #include "insns/vlsegstbu.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x20b)
  {
    reg_t npc = pc + insn_length(0x0000020b);
    #include "insns/vlbu.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_215(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_21f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_229(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_233(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x633)
  {
    reg_t npc = pc + insn_length(0x00000633);
    #include "insns/div.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x233)
  {
    reg_t npc = pc + insn_length(0x00000233);
    #include "insns/xor.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_23d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_247(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000047);
    #include "insns/fmsub_s.h"
    return npc;
}

reg_t processor_t::opcode_func_251(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_25b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_265(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_26f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_279(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_283(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000283);
    #include "insns/lhu.h"
    return npc;
}

reg_t processor_t::opcode_func_28d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_297(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2a1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_2ab(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2b5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_2bf(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2c9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_2d3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3ff1ff) == 0xc0d3)
  {
    reg_t npc = pc + insn_length(0x0000c0d3);
    #include "insns/fcvt_d_l.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xe0d3)
  {
    reg_t npc = pc + insn_length(0x0000e0d3);
    #include "insns/fcvt_d_w.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x100d3)
  {
    reg_t npc = pc + insn_length(0x000100d3);
    #include "insns/fcvt_d_s.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x20d3)
  {
    reg_t npc = pc + insn_length(0x000020d3);
    #include "insns/fmul_d.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xf0d3)
  {
    reg_t npc = pc + insn_length(0x0000f0d3);
    #include "insns/fcvt_d_wu.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xb0d3)
  {
    reg_t npc = pc + insn_length(0x0000b0d3);
    #include "insns/fcvt_wu_d.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xd0d3)
  {
    reg_t npc = pc + insn_length(0x0000d0d3);
    #include "insns/fcvt_d_lu.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0xa0d3)
  {
    reg_t npc = pc + insn_length(0x0000a0d3);
    #include "insns/fcvt_w_d.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x80d3)
  {
    reg_t npc = pc + insn_length(0x000080d3);
    #include "insns/fcvt_l_d.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0xd3)
  {
    reg_t npc = pc + insn_length(0x000000d3);
    #include "insns/fadd_d.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x90d3)
  {
    reg_t npc = pc + insn_length(0x000090d3);
    #include "insns/fcvt_lu_d.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x10d3)
  {
    reg_t npc = pc + insn_length(0x000010d3);
    #include "insns/fsub_d.h"
    return npc;
  }
  if((insn.bits & 0x3ff1ff) == 0x40d3)
  {
    reg_t npc = pc + insn_length(0x000040d3);
    #include "insns/fsqrt_d.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x30d3)
  {
    reg_t npc = pc + insn_length(0x000030d3);
    #include "insns/fdiv_d.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2dd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_2e7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_2f1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_2fb(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xffffffff) == 0x2fb)
  {
    reg_t npc = pc + insn_length(0x000002fb);
    #include "insns/cflush.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_305(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_30f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_319(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_323(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_32d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_337(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_341(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_34b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_355(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_35f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_369(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_373(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_37d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_387(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_391(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_39b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3a5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3af(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000003af);
    #include "insns/fence_g_cv.h"
    return npc;
}

reg_t processor_t::opcode_func_3b9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3c3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3cd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_3d7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3e1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_3eb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3f5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_3ff(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

