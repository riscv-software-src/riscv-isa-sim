require_supervisor;
RD = p->set_pcr(insn.rtype.rs1, p->get_pcr(insn.rtype.rs1) | SIMM);
