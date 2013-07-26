hwacha_disassembler::hwacha_disassembler()
{
  #define DEFINE_RS1(code) DISASM_INSN(#code, code, 0, xrs1_reg)
  #define DEFINE_RS1_RS2(code) DISASM_INSN(#code, code, 0, xrs1_reg, xrs2_reg)
  #define DEFINE_VEC_XMEM(code) DISASM_INSN(#code, code, 0, vxrd_reg, xrs1_reg)
  #define DEFINE_VEC_XMEMST(code) DISASM_INSN(#code, code, 0, vxrd_reg, xrs1_reg, xrs2_reg)
  #define DEFINE_VEC_FMEM(code) DISASM_INSN(#code, code, 0, vfrd_reg, xrs1_reg)
  #define DEFINE_VEC_FMEMST(code) DISASM_INSN(#code, code, 0, vfrd_reg, xrs1_reg, xrs2_reg)

  DEFINE_RS1(vxcptsave);
  DEFINE_RS1(vxcptrestore);
  DEFINE_NOARG(vxcptkill);

  DEFINE_RS1(vxcptevac);
  DEFINE_NOARG(vxcpthold);
  DEFINE_RS1_RS2(venqcmd);
  DEFINE_RS1_RS2(venqimm1);
  DEFINE_RS1_RS2(venqimm2);
  DEFINE_RS1_RS2(venqcnt);

  DEFINE_VEC_XMEM(vld);
  DEFINE_VEC_XMEM(vlw);
  DEFINE_VEC_XMEM(vlwu);
  DEFINE_VEC_XMEM(vlh);
  DEFINE_VEC_XMEM(vlhu);
  DEFINE_VEC_XMEM(vlb);
  DEFINE_VEC_XMEM(vlbu);
  DEFINE_VEC_FMEM(vfld);
  DEFINE_VEC_FMEM(vflw);
  DEFINE_VEC_XMEMST(vlstd);
  DEFINE_VEC_XMEMST(vlstw);
  DEFINE_VEC_XMEMST(vlstwu);
  DEFINE_VEC_XMEMST(vlsth);
  DEFINE_VEC_XMEMST(vlsthu);
  DEFINE_VEC_XMEMST(vlstb);
  DEFINE_VEC_XMEMST(vlstbu);
  DEFINE_VEC_FMEMST(vflstd);
  DEFINE_VEC_FMEMST(vflstw);

  DEFINE_VEC_XMEM(vsd);
  DEFINE_VEC_XMEM(vsw);
  DEFINE_VEC_XMEM(vsh);
  DEFINE_VEC_XMEM(vsb);
  DEFINE_VEC_FMEM(vfsd);
  DEFINE_VEC_FMEM(vfsw);
  DEFINE_VEC_XMEMST(vsstd);
  DEFINE_VEC_XMEMST(vsstw);
  DEFINE_VEC_XMEMST(vssth);
  DEFINE_VEC_XMEMST(vsstb);
  DEFINE_VEC_FMEMST(vfsstd);
  DEFINE_VEC_FMEMST(vfsstw);

  DISASM_INSN("vmvv", vmvv, 0, vxrd_reg, vxrs1_reg);
  DISASM_INSN("vmsv", vmsv, 0, vxrd_reg, xrs1_reg);
  DISASM_INSN("vmst", vmst, 0, vxrd_reg, xrs1_reg, xrs2_reg);
  DISASM_INSN("vmts", vmts, 0, xrd_reg, vxrs1_reg, xrs2_reg);
  DISASM_INSN("vfmvv", vfmvv, 0, vfrd_reg, vfrs1_reg);
  DISASM_INSN("vfmsv", vfmsv, 0, vfrd_reg, frs1_reg);
  DISASM_INSN("vfmst", vfmst, 0, vfrd_reg, frs1_reg, frs2_reg);
  DISASM_INSN("vfmts", vfmts, 0, frd_reg, vfrs1_reg, frs2_reg);

  DEFINE_RS1_RS2(vvcfg);
  DEFINE_RS1_RS2(vtcfg);

  DISASM_INSN("vvcfgivl", vvcfgivl, 0, xrd_reg, xrs1_reg, nxregs_reg, nfregs_reg);
  DISASM_INSN("vtcfgivl", vtcfgivl, 0, xrd_reg, xrs1_reg, nxregs_reg, nfregs_reg);
  DISASM_INSN("vsetvl", vsetvl, 0, xrd_reg, xrs1_reg);
  DISASM_INSN("vf", vf, 0, xrs1_reg, imm);

  DEFINE_NOARG(fence_v_l);
  DEFINE_NOARG(fence_v_g);
}
