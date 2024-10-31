#undef WRITE_RVC_RS1S
#undef RVC_RS1S
#undef RVC_RS2S
#undef RVC_FRS2S
#undef RVC_LW_IMM

#define WRITE_RVC_RS1S(value) WRITE_REG(insn.cswtype.rvc_rs1s, value)
#define RVC_RS1S READ_REG(insn.cswtype.rvc_rs1s)
#define RVC_RS2S READ_REG(insn.cswtype.rvc_rs2s)
#define RVC_FRS2S READ_FREG(insn.cswtype.rvc_rs2s)
#define RVC_LW_IMM (insn.cswtype.rvc_lw_imm)
