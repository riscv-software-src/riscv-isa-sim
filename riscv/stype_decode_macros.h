#undef RS1
#undef RS2
#undef S_IMM
#define RS1 READ_REG(insn.stype.rs1)
#define RS2 READ_REG(insn.stype.rs2)
#define S_IMM (insn.stype.s_imm)
