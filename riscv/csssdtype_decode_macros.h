#undef RVC_SDSP_IMM
#undef RVC_RS2
#undef RVC_FRS2

#define RVC_SDSP_IMM (insn.csssdtype.rvc_sdsp_imm)
#define RVC_RS2 READ_REG(insn.csssdtype.rvc_rs2)
#define RVC_FRS2 READ_FREG(insn.csssdtype.rvc_rs2)
