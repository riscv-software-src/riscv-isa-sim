#undef RD
#undef RS1
#undef RS2

#define RD READ_REG(insn.rtype.rd)
#define RS1 READ_REG(insn.rtype.rs1)
#define RS2 READ_REG(insn.rtype.rs2)
