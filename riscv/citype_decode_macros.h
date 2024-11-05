#undef WRITE_RD
#undef WRITE_RVC_RS1S
#undef RVC_RS1
#undef RVC_IMM
#undef RVC_ZIMM
#undef RVC_ADDI16SP_IMM
#define WRITE_RD(value) WRITE_REG(insn.citype.rvc_rd, value)
#define WRITE_RVC_RS1S(value) WRITE_REG(insn.citype.rvc_rs1s, value)
#define RVC_RS1 READ_REG(insn.citype.rvc_rs1)
#define RVC_IMM (insn.citype.rvc_multi_imm)
#define RVC_ZIMM (insn.citype.rvc_multi_imm & 0x3f)
#define RVC_ADDI16SP_IMM (insn.citype.rvc_multi_imm)
