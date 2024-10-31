#undef RVC_RS1S
#undef RVC_RS2S
#undef RVC_LD_IMM
#undef WRITE_RVC_RS2S
#undef WRITE_RVC_FRS2S

#define RVC_RS1S READ_REG(insn.cldtype.rvc_rs1s)
#define RVC_RS2S READ_REG(insn.cldtype.rvc_rs2s)
#define RVC_LD_IMM (insn.cldtype.rvc_ld_imm)
#define WRITE_RVC_RS2S(value) WRITE_REG(insn.cldtype.rvc_rs2s, value)
#define WRITE_RVC_FRS2S(value) WRITE_FREG(insn.cldtype.rvc_rs2s, value)
