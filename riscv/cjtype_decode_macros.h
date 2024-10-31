#undef RVC_J_IMM
#undef RVC_IMM
#undef RVC_RS1
#undef WRITE_RD
#define RVC_J_IMM (insn.cjtype.rvc_j_imm)
#define RVC_IMM (insn.cjtype.rvc_imm)
#define RVC_RS1 READ_REG(insn.cjtype.rvc_rs1)
#define WRITE_RD(value) WRITE_REG(insn.cjtype.rvc_rd, value)
