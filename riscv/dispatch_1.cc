#include "insn_includes.h"

reg_t processor_t::opcode_func_001(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_00b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3fffff) == 0xb)
  {
    reg_t npc = pc + insn_length(0x0000000b);
    #include "insns/vlb.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x100b)
  {
    reg_t npc = pc + insn_length(0x0000100b);
    #include "insns/vlstb.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0x80b)
  {
    reg_t npc = pc + insn_length(0x0000080b);
    #include "insns/vlsegstb.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x200b)
  {
    reg_t npc = pc + insn_length(0x0000200b);
    #include "insns/vlsegb.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_015(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_01f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_029(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_033(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x33)
  {
    reg_t npc = pc + insn_length(0x00000033);
    #include "insns/add.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x433)
  {
    reg_t npc = pc + insn_length(0x00000433);
    #include "insns/mul.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x10033)
  {
    reg_t npc = pc + insn_length(0x00010033);
    #include "insns/sub.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_03d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_047(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000047);
    #include "insns/fmsub_s.h"
    return npc;
}

reg_t processor_t::opcode_func_051(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_05b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_065(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_06f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_079(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_083(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000083);
    #include "insns/lh.h"
    return npc;
}

reg_t processor_t::opcode_func_08d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_097(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0a1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_0ab(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0b5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_0bf(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0c9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_0d3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x180d3)
  {
    reg_t npc = pc + insn_length(0x000180d3);
    #include "insns/fmin_d.h"
    return npc;
  }
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
  if((insn.bits & 0x1ffff) == 0x190d3)
  {
    reg_t npc = pc + insn_length(0x000190d3);
    #include "insns/fmax_d.h"
    return npc;
  }
  if((insn.bits & 0x7c1ffff) == 0x1c0d3)
  {
    reg_t npc = pc + insn_length(0x0001c0d3);
    #include "insns/mftx_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x170d3)
  {
    reg_t npc = pc + insn_length(0x000170d3);
    #include "insns/fle_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x160d3)
  {
    reg_t npc = pc + insn_length(0x000160d3);
    #include "insns/flt_d.h"
    return npc;
  }
  if((insn.bits & 0x1f1ff) == 0x20d3)
  {
    reg_t npc = pc + insn_length(0x000020d3);
    #include "insns/fmul_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x70d3)
  {
    reg_t npc = pc + insn_length(0x000070d3);
    #include "insns/fsgnjx_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x150d3)
  {
    reg_t npc = pc + insn_length(0x000150d3);
    #include "insns/feq_d.h"
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
  if((insn.bits & 0x1ffff) == 0x60d3)
  {
    reg_t npc = pc + insn_length(0x000060d3);
    #include "insns/fsgnjn_d.h"
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
  if((insn.bits & 0x3fffff) == 0x1e0d3)
  {
    reg_t npc = pc + insn_length(0x0001e0d3);
    #include "insns/mxtf_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x50d3)
  {
    reg_t npc = pc + insn_length(0x000050d3);
    #include "insns/fsgnj_d.h"
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

reg_t processor_t::opcode_func_0dd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_0e7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_0f1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_0fb(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x7ffffff) == 0xfb)
  {
    reg_t npc = pc + insn_length(0x000000fb);
    #include "insns/di.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_105(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_10f(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x150f)
  {
    reg_t npc = pc + insn_length(0x0000150f);
    #include "insns/vfsstw.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0x90f)
  {
    reg_t npc = pc + insn_length(0x0000090f);
    #include "insns/vssegstw.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x10f)
  {
    reg_t npc = pc + insn_length(0x0000010f);
    #include "insns/vsw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x110f)
  {
    reg_t npc = pc + insn_length(0x0000110f);
    #include "insns/vsstw.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0xd0f)
  {
    reg_t npc = pc + insn_length(0x00000d0f);
    #include "insns/vfssegstw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x210f)
  {
    reg_t npc = pc + insn_length(0x0000210f);
    #include "insns/vssegw.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x50f)
  {
    reg_t npc = pc + insn_length(0x0000050f);
    #include "insns/vfsw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x250f)
  {
    reg_t npc = pc + insn_length(0x0000250f);
    #include "insns/vfssegw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_119(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_123(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000123);
    #include "insns/sw.h"
    return npc;
}

reg_t processor_t::opcode_func_12d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_137(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_141(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_14b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_155(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_15f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_169(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_173(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x1173)
  {
    reg_t npc = pc + insn_length(0x00001173);
    #include "insns/vfmst.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x1973)
  {
    reg_t npc = pc + insn_length(0x00001973);
    #include "insns/vfmts.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x973)
  {
    reg_t npc = pc + insn_length(0x00000973);
    #include "insns/vfmsv.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x173)
  {
    reg_t npc = pc + insn_length(0x00000173);
    #include "insns/vfmvv.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_17d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_187(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000187);
    #include "insns/fld.h"
    return npc;
}

reg_t processor_t::opcode_func_191(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_19b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1a5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_1af(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1b9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1c3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1cd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_1d7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1e1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_1eb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1f5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_1ff(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_209(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_213(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000213);
    #include "insns/xori.h"
    return npc;
}

reg_t processor_t::opcode_func_21d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_227(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_231(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_23b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x63b)
  {
    reg_t npc = pc + insn_length(0x0000063b);
    #include "insns/divw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_245(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_24f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000004f);
    #include "insns/fnmadd_s.h"
    return npc;
}

reg_t processor_t::opcode_func_259(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_263(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000263);
    #include "insns/blt.h"
    return npc;
}

reg_t processor_t::opcode_func_26d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_277(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x7ffffff) == 0x277)
  {
    reg_t npc = pc + insn_length(0x00000277);
    #include "insns/rdcycle.h"
    return npc;
  }
  if((insn.bits & 0x7ffffff) == 0xa77)
  {
    reg_t npc = pc + insn_length(0x00000a77);
    #include "insns/rdinstret.h"
    return npc;
  }
  if((insn.bits & 0x7ffffff) == 0x677)
  {
    reg_t npc = pc + insn_length(0x00000677);
    #include "insns/rdtime.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_281(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_28b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x128b)
  {
    reg_t npc = pc + insn_length(0x0000128b);
    #include "insns/vlsthu.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0xa8b)
  {
    reg_t npc = pc + insn_length(0x00000a8b);
    #include "insns/vlsegsthu.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x28b)
  {
    reg_t npc = pc + insn_length(0x0000028b);
    #include "insns/vlhu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x228b)
  {
    reg_t npc = pc + insn_length(0x0000228b);
    #include "insns/vlseghu.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_295(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_29f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2a9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_2b3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x102b3)
  {
    reg_t npc = pc + insn_length(0x000102b3);
    #include "insns/sra.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x2b3)
  {
    reg_t npc = pc + insn_length(0x000002b3);
    #include "insns/srl.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x6b3)
  {
    reg_t npc = pc + insn_length(0x000006b3);
    #include "insns/divu.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2bd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_2c7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000c7);
    #include "insns/fmsub_d.h"
    return npc;
}

reg_t processor_t::opcode_func_2d1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_2db(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2e5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2ef(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_2f9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_303(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000303);
    #include "insns/lwu.h"
    return npc;
}

reg_t processor_t::opcode_func_30d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_317(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_321(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_32b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_335(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_33f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_349(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_353(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_35d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_367(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_371(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_37b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_385(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_38f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_399(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_3a3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3ad(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_3b7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_3c1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_3cb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3d5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_3df(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3e9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_3f3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xf80003ff) == 0x3f3)
  {
    reg_t npc = pc + insn_length(0x000003f3);
    #include "insns/vf.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3fd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

