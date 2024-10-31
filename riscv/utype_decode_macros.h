#undef WRITE_RD
#undef U_IMM

#define WRITE_RD(value) WRITE_REG(insn.utype.rd, value)
#define U_IMM (insn.utype.u_imm)
