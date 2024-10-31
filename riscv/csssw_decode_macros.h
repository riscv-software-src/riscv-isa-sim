#undef RVC_SWSP_IMM
#undef RVC_RS2
#undef RVC_FRS2

#define RVC_SWSP_IMM (insn.cssswtype.rvc_swsp_imm)
#define RVC_RS2 READ_REG(insn.cssswtype.rvc_rs2)
#define RVC_FRS2 READ_FREG(insn.cssswtype.rvc_rs2)
