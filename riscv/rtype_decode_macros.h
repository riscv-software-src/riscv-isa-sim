#undef WRITE_RD
#undef RS1
#undef RS2

#define WRITE_RD(value) WRITE_REG(insn.rtype.rd, value)
#define RS1 READ_REG(insn.rtype.rs1)
#define RS2 READ_REG(insn.rtype.rs2)
