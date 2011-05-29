#include "insn_includes.h"

reg_t processor_t::opcode_func_007(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_011(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_01b(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001b);
    #include "insns/addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_025(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_02f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_039(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_043(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000043);
    #include "insns/fmadd_s.h"
    return npc;
}

reg_t processor_t::opcode_func_04d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_057(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_061(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_06b(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006b);
    #include "insns/jalr_c.h"
    return npc;
}

reg_t processor_t::opcode_func_075(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_07f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_089(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_093(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3f03ff) == 0x93)
  {
    reg_t npc = pc + insn_length(0x00000093);
    #include "insns/slli.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_09d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_0a7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0b1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_0bb(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0xbb)
  {
    reg_t npc = pc + insn_length(0x000000bb);
    #include "insns/sllw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0c5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0cf(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000cf);
    #include "insns/fnmadd_d.h"
    return npc;
}

reg_t processor_t::opcode_func_0d9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_0e3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000e3);
    #include "insns/bne.h"
    return npc;
}

reg_t processor_t::opcode_func_0ed(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_0f7(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xffffffff) == 0xf7)
  {
    reg_t npc = pc + insn_length(0x000000f7);
    #include "insns/break.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_101(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_10b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x150b)
  {
    reg_t npc = pc + insn_length(0x0000150b);
    #include "insns/vflstw.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x10b)
  {
    reg_t npc = pc + insn_length(0x0000010b);
    #include "insns/vlw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x110b)
  {
    reg_t npc = pc + insn_length(0x0000110b);
    #include "insns/vlstw.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0x90b)
  {
    reg_t npc = pc + insn_length(0x0000090b);
    #include "insns/vlsegstw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x250b)
  {
    reg_t npc = pc + insn_length(0x0000250b);
    #include "insns/vflsegw.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0xd0b)
  {
    reg_t npc = pc + insn_length(0x00000d0b);
    #include "insns/vflsegstw.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x50b)
  {
    reg_t npc = pc + insn_length(0x0000050b);
    #include "insns/vflw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x210b)
  {
    reg_t npc = pc + insn_length(0x0000210b);
    #include "insns/vlsegw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_115(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_11f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_129(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_133(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x533)
  {
    reg_t npc = pc + insn_length(0x00000533);
    #include "insns/mulhsu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x133)
  {
    reg_t npc = pc + insn_length(0x00000133);
    #include "insns/slt.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_13d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_147(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_151(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_15b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_165(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_16f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_179(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_183(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000183);
    #include "insns/ld.h"
    return npc;
}

reg_t processor_t::opcode_func_18d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_197(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1a1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_1ab(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x19ab)
  {
    reg_t npc = pc + insn_length(0x000019ab);
    #include "insns/amominu_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x9ab)
  {
    reg_t npc = pc + insn_length(0x000009ab);
    #include "insns/amoand_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x1dab)
  {
    reg_t npc = pc + insn_length(0x00001dab);
    #include "insns/amomaxu_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x1ab)
  {
    reg_t npc = pc + insn_length(0x000001ab);
    #include "insns/amoadd_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x15ab)
  {
    reg_t npc = pc + insn_length(0x000015ab);
    #include "insns/amomax_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0xdab)
  {
    reg_t npc = pc + insn_length(0x00000dab);
    #include "insns/amoor_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x5ab)
  {
    reg_t npc = pc + insn_length(0x000005ab);
    #include "insns/amoswap_d.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x11ab)
  {
    reg_t npc = pc + insn_length(0x000011ab);
    #include "insns/amomin_d.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1b5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_1bf(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1c9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_1d3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1dd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_1e7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_1f1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_1fb(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xf801ffff) == 0x1fb)
  {
    reg_t npc = pc + insn_length(0x000001fb);
    #include "insns/mtpcr.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_205(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_20f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_219(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_223(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_22d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_237(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_241(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_24b(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000004b);
    #include "insns/fnmsub_s.h"
    return npc;
}

reg_t processor_t::opcode_func_255(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_25f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_269(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_273(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_27d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_287(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_291(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_29b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3f83ff) == 0x29b)
  {
    reg_t npc = pc + insn_length(0x0000029b);
    #include "insns/srliw.h"
    return npc;
  }
  if((insn.bits & 0x3f83ff) == 0x1029b)
  {
    reg_t npc = pc + insn_length(0x0001029b);
    #include "insns/sraiw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2a5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2af(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000002af);
    #include "insns/fence_g_v.h"
    return npc;
}

reg_t processor_t::opcode_func_2b9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2c3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000c3);
    #include "insns/fmadd_d.h"
    return npc;
}

reg_t processor_t::opcode_func_2cd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_2d7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2e1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_2eb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2f5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_2ff(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_309(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_313(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000313);
    #include "insns/ori.h"
    return npc;
}

reg_t processor_t::opcode_func_31d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_327(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_331(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_33b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x73b)
  {
    reg_t npc = pc + insn_length(0x0000073b);
    #include "insns/remw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_345(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_34f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_359(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_363(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000363);
    #include "insns/bltu.h"
    return npc;
}

reg_t processor_t::opcode_func_36d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_377(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_381(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_38b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_395(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_39f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3a9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_3b3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x7b3)
  {
    reg_t npc = pc + insn_length(0x000007b3);
    #include "insns/remu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x3b3)
  {
    reg_t npc = pc + insn_length(0x000003b3);
    #include "insns/and.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3bd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_3c7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3d1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_3db(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3e5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_3ef(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_3f9(insn_t insn, reg_t pc)
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

