#include "hwacha.h"

static const char* xpr[] = {
  "zero", "ra", "s0", "s1",  "s2",  "s3",  "s4",  "s5",
  "s6",   "s7", "s8", "s9", "s10", "s11",  "sp",  "tp",
  "v0",   "v1", "a0", "a1",  "a2",  "a3",  "a4",  "a5",
  "a6",   "a7", "t0", "t1",  "t2",  "t3",  "t4",  "gp"
};

static const char* fpr[] = {
  "fs0", "fs1",  "fs2",  "fs3",  "fs4",  "fs5",  "fs6",  "fs7",
  "fs8", "fs9", "fs10", "fs11", "fs12", "fs13", "fs14", "fs15",
  "fv0", "fv1", "fa0",   "fa1",  "fa2",  "fa3",  "fa4",  "fa5",
  "fa6", "fa7", "ft0",   "ft1",  "ft2",  "ft3",  "ft4",  "ft5"
};

static const char* vxpr[] = {
  "vx0",  "vx1",  "vx2",  "vx3",  "vx4",  "vx5",  "vx6",  "vx7",
  "vx8",  "vx9",  "vx10", "vx11", "vx12", "vx13", "vx14", "vx15",
  "vx16", "vx17", "vx18", "vx19", "vx20", "vx21", "vx22", "vx23",
  "vx24", "vx25", "vx26", "vx27", "vx28", "vx29", "vx30", "vx31"
};

static const char* vfpr[] = {
  "vf0",  "vf1",  "vf2",  "vf3",  "vf4",  "vf5",  "vf6",  "vf7",
  "vf8",  "vf9",  "vf10", "vf11", "vf12", "vf13", "vf14", "vf15",
  "vf16", "vf17", "vf18", "vf19", "vf20", "vf21", "vf22", "vf23",
  "vf24", "vf25", "vf26", "vf27", "vf28", "vf29", "vf30", "vf31"
};

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr[insn.rs1()];
  }
} xrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr[insn.rs2()];
  }
} xrs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return xpr[insn.rd()];
  }
} xrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rd()];
  }
} frd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rs1()];
  }
} frs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rs2()];
  }
} frs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return fpr[insn.rs3()];
  }
} frs3;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vxpr[insn.rd()];
  }
} vxrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vxpr[insn.rs1()];
  }
} vxrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vfpr[insn.rd()];
  }
} vfrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return vfpr[insn.rs1()];
  }
} vfrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string(insn.i_imm() & 0x3f);
  }
} nxregs;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((insn.i_imm() >> 6) & 0x3f);
  }
} nfregs;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.s_imm()) + '(' + xpr[insn.rs1()] + ')';
  }
} vf_addr;

std::vector<disasm_insn_t*> hwacha_t::get_disasms()
{
  std::vector<disasm_insn_t*> insns;

  #define DECLARE_INSN(code, match, mask) \
   const uint32_t match_##code = match; \
   const uint32_t mask_##code = mask;
  #include "opcodes_hwacha.h"
  #undef DECLARE_INSN

  #define DISASM_INSN(name, code, extra, ...) \
    insns.push_back(new disasm_insn_t(name, match_##code, mask_##code | (extra), __VA_ARGS__));

  DISASM_INSN("vsetcfg", vsetcfg, 0, {&xrs1, &nxregs, &nfregs});
  DISASM_INSN("vsetvl", vsetvl, 0, {&xrd, &xrs1});
  DISASM_INSN("vgetcfg", vgetcfg, 0, {&xrd});
  DISASM_INSN("vgetvl", vgetvl, 0, {&xrd});

  DISASM_INSN("vmvv", vmvv, 0, {&vxrd, &vxrs1});
  DISASM_INSN("vmsv", vmsv, 0, {&vxrd, &xrs1});
  DISASM_INSN("vfmvv", vfmvv, 0, {&vfrd, &vfrs1});
  DISASM_INSN("vfmsv.s", vfmsv_s, 0, {&vfrd, &xrs1});
  DISASM_INSN("vfmsv.d", vfmsv_d, 0, {&vfrd, &xrs1});
  DISASM_INSN("vf", vf, 0, {&vf_addr});

  DISASM_INSN("vxcptcause", vxcptcause, 0, {&xrd});
  DISASM_INSN("vxcptaux", vxcptaux, 0, {&xrd});
  DISASM_INSN("vxcptsave", vxcptsave, 0, {&xrs1});
  DISASM_INSN("vxcptrestore", vxcptrestore, 0, {&xrs1});
  DISASM_INSN("vxcptkill", vxcptkill, 0, {});

  const uint32_t mask_vseglen = 0x7UL << 29;

  #define DISASM_VMEM_INSN(name1, name2, code, ...) \
    DISASM_INSN(name1, code, mask_vseglen, __VA_ARGS__) \
    DISASM_INSN(name2, code, 0, __VA_ARGS__) \

  DISASM_VMEM_INSN("vld", "vlsegd", vlsegd, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vlw", "vlsegw", vlsegw, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vlwu", "vlsegwu", vlsegwu, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vlh", "vlsegh", vlsegh, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vlhu", "vlseghu", vlseghu, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vlb", "vlsegb", vlsegb, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vlbu", "vlsegbu", vlsegbu, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vfld", "vflsegd", vflsegd, {&vfrd, &xrs1});
  DISASM_VMEM_INSN("vflw", "vflsegw", vflsegw, {&vfrd, &xrs1});

  DISASM_VMEM_INSN("vlstd", "vlsegstd", vlsegstd, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vlstw", "vlsegstw", vlsegstw, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vlstwu", "vlsegstwu", vlsegstwu, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vlsth", "vlsegsth", vlsegsth, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vlsthu", "vlsegsthu", vlsegsthu, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vlstb", "vlsegstb", vlsegstb, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vlstbu", "vlsegstbu", vlsegstbu, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vflstd", "vflsegstd", vflsegstd, {&vfrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vflstw", "vflsegstw", vflsegstw, {&vfrd, &xrs1, &xrs2});

  DISASM_VMEM_INSN("vsd", "vssegd", vssegd, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vsw", "vssegw", vssegw, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vsh", "vssegh", vssegh, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vsb", "vssegb", vssegb, {&vxrd, &xrs1});
  DISASM_VMEM_INSN("vfsd", "vfssegd", vfssegd, {&vfrd, &xrs1});
  DISASM_VMEM_INSN("vfsw", "vfssegw", vfssegw, {&vfrd, &xrs1});

  DISASM_VMEM_INSN("vsstd", "vssegstd", vssegstd, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vsstw", "vssegstw", vssegstw, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vssth", "vssegsth", vssegsth, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vsstb", "vssegstb", vssegstb, {&vxrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vfsstd", "vfssegstd", vfssegstd, {&vfrd, &xrs1, &xrs2});
  DISASM_VMEM_INSN("vfsstw", "vfssegstw", vfssegstw, {&vfrd, &xrs1, &xrs2});

  #define DECLARE_INSN(code, match, mask) \
   const uint32_t match_##code = match; \
   const uint32_t mask_##code = mask;
  #include "opcodes_hwacha_ut.h"
  #undef DECLARE_INSN

  #define DISASM_UT_INSN(name, code, extra, ...) \
    ut_disassembler.add_insn(new disasm_insn_t(name, match_##code, mask_##code | (extra), __VA_ARGS__));

  DISASM_UT_INSN("stop", ut_stop, 0, {});
  DISASM_UT_INSN("utidx", ut_utidx, 0, {&xrd});
  DISASM_UT_INSN("movz", ut_movz, 0, {&xrd, &xrs1, &xrs2});
  DISASM_UT_INSN("movn", ut_movn, 0, {&xrd, &xrs1, &xrs2});
  DISASM_UT_INSN("fmovz", ut_fmovz, 0, {&frd, &xrs1, &frs2});
  DISASM_UT_INSN("fmovn", ut_fmovn, 0, {&frd, &xrs1, &frs2});

  return insns;
}
