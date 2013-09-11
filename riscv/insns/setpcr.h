require_supervisor;
RD = p->set_pcr(insn.rs1(), p->get_pcr(insn.rs1()) | insn.i_imm());
