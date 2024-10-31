#undef RVC_RS1S
#undef RVC_RS2S
#undef RVC_LW_IMM
#undef WRITE_RVC_RS2S
#undef WRITE_RVC_FRS2S

#define RVC_RS1S READ_REG(insn.clwtype.rvc_rs1s)
#define RVC_RS2S READ_REG(insn.clwtype.rvc_rs2s)
#define RVC_LW_IMM (insn.clwtype.rvc_lw_imm)
#define WRITE_RVC_RS2S(value) WRITE_REG(insn.clwtype.rvc_rs2s, value)
#define WRITE_RVC_FRS2S(value) WRITE_FREG(insn.clwtype.rvc_rs2s, value)
