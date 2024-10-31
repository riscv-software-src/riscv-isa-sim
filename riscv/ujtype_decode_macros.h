#undef WRITE_RD
#undef JUMP_TARGET
#define WRITE_RD(value) WRITE_REG(insn.ujtype.rd, value)
#define JUMP_TARGET (pc + insn.ujtype.uj_imm)
