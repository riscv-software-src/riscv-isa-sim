#undef RVC_LDSP_IMM
#undef WRITE_FRD

#define RVC_LDSP_IMM (insn.csstype.rvc_ldsp_imm)
#define WRITE_FRD(value) WRITE_FREG(insn.csstype.rvc_rd, value)
