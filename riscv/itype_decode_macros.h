#undef WRITE_RD
#undef RS1
#undef I_IMM

#define WRITE_RD(value) WRITE_REG(insn.itype.rd, value)
#define RS1 READ_REG(insn.itype.rs1)
#define I_IMM (insn.itype.i_imm)
