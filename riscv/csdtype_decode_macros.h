#undef WRITE_RVC_RS1S
#undef RVC_RS1S
#undef RVC_RS2S
#undef RVC_FRS2S
#undef RVC_LD_IMM

#define WRITE_RVC_RS1S(value) WRITE_REG(insn.csdtype.rvc_rs1s, value)
#define RVC_RS1S READ_REG(insn.csdtype.rvc_rs1s)
#define RVC_RS2S READ_REG(insn.csdtype.rvc_rs2s)
#define RVC_FRS2S READ_FREG(insn.csdtype.rvc_rs2s)
#define RVC_LD_IMM (insn.csdtype.rvc_ld_imm)
