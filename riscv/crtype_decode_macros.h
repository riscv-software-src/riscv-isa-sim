#undef WRITE_RD
#undef RVC_RS1
#undef RVC_RS2
#define WRITE_RD(value) WRITE_REG(insn.crtype.rd, value)
#define RVC_RS1 READ_REG(insn.crtype.rvc_rs1)
#define RVC_RS2 READ_REG(insn.crtype.rvc_rs2)
