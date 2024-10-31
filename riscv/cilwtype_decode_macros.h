#undef RVC_LWSP_IMM
#undef WRITE_FRD

#define RVC_LWSP_IMM (insn.cilwtype.rvc_lwsp_imm)
#define WRITE_FRD(value) WRITE_FREG(insn.cilwtype.rvc_rd, value)
