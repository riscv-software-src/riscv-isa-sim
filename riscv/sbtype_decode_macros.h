#undef RS1
#undef RS2
#undef BRANCH_TARGET
#define RS1 READ_REG(insn.sbtype.rs1)
#define RS2 READ_REG(insn.sbtype.rs2)
#define BRANCH_TARGET (pc + insn.sbtype.sb_imm)
