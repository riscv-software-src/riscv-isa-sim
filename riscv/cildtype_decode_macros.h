#undef RVC_LDSP_IMM
#undef WRITE_RD
#undef WRITE_FRD

#define RVC_LDSP_IMM (insn.cildtype.rvc_ldsp_imm)
#define WRITE_RD(value) WRITE_REG(insn.cildtype.rvc_rd, value)
#define WRITE_FRD(value) WRITE_FREG(insn.cildtype.rvc_rd, value)
