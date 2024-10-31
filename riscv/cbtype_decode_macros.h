#undef WRITE_RVC_RS1S
#undef RVC_RS1S
#undef RVC_IMM

#define WRITE_RVC_RS1S(value) WRITE_REG(insn.cbtype.rvc_rs1s, value)
#define RVC_RS1S READ_REG(insn.cbtype.rvc_rs1s)
#define RVC_IMM (insn.cbtype.rvc_imm)
