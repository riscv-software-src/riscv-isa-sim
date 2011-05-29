#include "insn_includes.h"

reg_t processor_t::opcode_func_009(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_013(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000013);
    #include "insns/addi.h"
    return npc;
}

reg_t processor_t::opcode_func_01d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_027(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_031(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_03b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x43b)
  {
    reg_t npc = pc + insn_length(0x0000043b);
    #include "insns/mulw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x3b)
  {
    reg_t npc = pc + insn_length(0x0000003b);
    #include "insns/addw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x1003b)
  {
    reg_t npc = pc + insn_length(0x0001003b);
    #include "insns/subw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_045(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_04f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000004f);
    #include "insns/fnmadd_s.h"
    return npc;
}

reg_t processor_t::opcode_func_059(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_063(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000063);
    #include "insns/beq.h"
    return npc;
}

reg_t processor_t::opcode_func_06d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_077(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xffffffff) == 0x77)
  {
    reg_t npc = pc + insn_length(0x00000077);
    #include "insns/syscall.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_081(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_08b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3fffff) == 0x8b)
  {
    reg_t npc = pc + insn_length(0x0000008b);
    #include "insns/vlh.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x108b)
  {
    reg_t npc = pc + insn_length(0x0000108b);
    #include "insns/vlsth.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0x88b)
  {
    reg_t npc = pc + insn_length(0x0000088b);
    #include "insns/vlsegsth.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x208b)
  {
    reg_t npc = pc + insn_length(0x0000208b);
    #include "insns/vlsegh.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_095(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_09f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0a9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_0b3(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0xb3)
  {
    reg_t npc = pc + insn_length(0x000000b3);
    #include "insns/sll.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x4b3)
  {
    reg_t npc = pc + insn_length(0x000004b3);
    #include "insns/mulh.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0bd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_0c7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000c7);
    #include "insns/fmsub_d.h"
    return npc;
}

reg_t processor_t::opcode_func_0d1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_0db(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_0e5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_0ef(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_0f9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_103(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000103);
    #include "insns/lw.h"
    return npc;
}

reg_t processor_t::opcode_func_10d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_117(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_121(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_12b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x192b)
  {
    reg_t npc = pc + insn_length(0x0000192b);
    #include "insns/amominu_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x92b)
  {
    reg_t npc = pc + insn_length(0x0000092b);
    #include "insns/amoand_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x1d2b)
  {
    reg_t npc = pc + insn_length(0x00001d2b);
    #include "insns/amomaxu_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x152b)
  {
    reg_t npc = pc + insn_length(0x0000152b);
    #include "insns/amomax_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x12b)
  {
    reg_t npc = pc + insn_length(0x0000012b);
    #include "insns/amoadd_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0xd2b)
  {
    reg_t npc = pc + insn_length(0x00000d2b);
    #include "insns/amoor_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x112b)
  {
    reg_t npc = pc + insn_length(0x0000112b);
    #include "insns/amomin_w.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x52b)
  {
    reg_t npc = pc + insn_length(0x0000052b);
    #include "insns/amoswap_w.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_135(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_13f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_149(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_153(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_15d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_167(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_171(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_17b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x7c1ffff) == 0x17b)
  {
    reg_t npc = pc + insn_length(0x0000017b);
    #include "insns/mfpcr.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_185(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_18f(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xfff) == 0x98f)
  {
    reg_t npc = pc + insn_length(0x0000098f);
    #include "insns/vssegstd.h"
    return npc;
  }
  if((insn.bits & 0xfff) == 0xd8f)
  {
    reg_t npc = pc + insn_length(0x00000d8f);
    #include "insns/vfssegstd.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x118f)
  {
    reg_t npc = pc + insn_length(0x0000118f);
    #include "insns/vsstd.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x218f)
  {
    reg_t npc = pc + insn_length(0x0000218f);
    #include "insns/vssegd.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x158f)
  {
    reg_t npc = pc + insn_length(0x0000158f);
    #include "insns/vfsstd.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x18f)
  {
    reg_t npc = pc + insn_length(0x0000018f);
    #include "insns/vsd.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x58f)
  {
    reg_t npc = pc + insn_length(0x0000058f);
    #include "insns/vfsd.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x258f)
  {
    reg_t npc = pc + insn_length(0x0000258f);
    #include "insns/vfssegd.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_199(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_1a3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000001a3);
    #include "insns/sd.h"
    return npc;
}

reg_t processor_t::opcode_func_1ad(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_1b7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000037);
    #include "insns/lui.h"
    return npc;
}

reg_t processor_t::opcode_func_1c1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_1cb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1d5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_1df(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_1e9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_1f3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000001f3);
    #include "insns/vtcfgivl.h"
    return npc;
}

reg_t processor_t::opcode_func_1fd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_207(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_211(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_21b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_225(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_22f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000022f);
    #include "insns/fence_l_v.h"
    return npc;
}

reg_t processor_t::opcode_func_239(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_243(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000043);
    #include "insns/fmadd_s.h"
    return npc;
}

reg_t processor_t::opcode_func_24d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_257(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_261(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_26b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x7ffffff) == 0x26b)
  {
    reg_t npc = pc + insn_length(0x0000026b);
    #include "insns/rdnpc.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_275(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_27f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_289(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_293(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x3f03ff) == 0x293)
  {
    reg_t npc = pc + insn_length(0x00000293);
    #include "insns/srli.h"
    return npc;
  }
  if((insn.bits & 0x3f03ff) == 0x10293)
  {
    reg_t npc = pc + insn_length(0x00010293);
    #include "insns/srai.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_29d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_2a7(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2b1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_2bb(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x6bb)
  {
    reg_t npc = pc + insn_length(0x000006bb);
    #include "insns/divuw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x2bb)
  {
    reg_t npc = pc + insn_length(0x000002bb);
    #include "insns/srlw.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x102bb)
  {
    reg_t npc = pc + insn_length(0x000102bb);
    #include "insns/sraw.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_2c5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_2cf(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000000cf);
    #include "insns/fnmadd_d.h"
    return npc;
}

reg_t processor_t::opcode_func_2d9(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_2e3(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x000002e3);
    #include "insns/bge.h"
    return npc;
}

reg_t processor_t::opcode_func_2ed(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_2f7(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x6f7)
  {
    reg_t npc = pc + insn_length(0x000006f7);
    #include "insns/movn.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x2f7)
  {
    reg_t npc = pc + insn_length(0x000002f7);
    #include "insns/movz.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0xef7)
  {
    reg_t npc = pc + insn_length(0x00000ef7);
    #include "insns/fmovn.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0xaf7)
  {
    reg_t npc = pc + insn_length(0x00000af7);
    #include "insns/fmovz.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_301(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_30b(insn_t insn, reg_t pc)
{
  if((insn.bits & 0xfff) == 0xb0b)
  {
    reg_t npc = pc + insn_length(0x00000b0b);
    #include "insns/vlsegstwu.h"
    return npc;
  }
  if((insn.bits & 0x3fffff) == 0x30b)
  {
    reg_t npc = pc + insn_length(0x0000030b);
    #include "insns/vlwu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x130b)
  {
    reg_t npc = pc + insn_length(0x0000130b);
    #include "insns/vlstwu.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x230b)
  {
    reg_t npc = pc + insn_length(0x0000230b);
    #include "insns/vlsegwu.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_315(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_31f(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_329(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_333(insn_t insn, reg_t pc)
{
  if((insn.bits & 0x1ffff) == 0x733)
  {
    reg_t npc = pc + insn_length(0x00000733);
    #include "insns/rem.h"
    return npc;
  }
  if((insn.bits & 0x1ffff) == 0x333)
  {
    reg_t npc = pc + insn_length(0x00000333);
    #include "insns/or.h"
    return npc;
  }
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_33d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_347(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_351(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_35b(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_365(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000005);
    #include "insns/c_lwsp.h"
    return npc;
}

reg_t processor_t::opcode_func_36f(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000006f);
    #include "insns/jal.h"
    return npc;
}

reg_t processor_t::opcode_func_379(insn_t insn, reg_t pc)
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

reg_t processor_t::opcode_func_383(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_38d(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000000d);
    #include "insns/c_sw.h"
    return npc;
}

reg_t processor_t::opcode_func_397(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3a1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000001);
    #include "insns/c_addi.h"
    return npc;
}

reg_t processor_t::opcode_func_3ab(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3b5(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000015);
    #include "insns/c_fld.h"
    return npc;
}

reg_t processor_t::opcode_func_3bf(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3c9(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000009);
    #include "insns/c_ld.h"
    return npc;
}

reg_t processor_t::opcode_func_3d3(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

reg_t processor_t::opcode_func_3dd(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x0000001d);
    #include "insns/c_addiw.h"
    return npc;
}

reg_t processor_t::opcode_func_3e7(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000067);
    #include "insns/j.h"
    return npc;
}

reg_t processor_t::opcode_func_3f1(insn_t insn, reg_t pc)
{
    reg_t npc = pc + insn_length(0x00000011);
    #include "insns/c_bne.h"
    return npc;
}

reg_t processor_t::opcode_func_3fb(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

