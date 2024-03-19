int csr = validate_csr(insn.csr(), true);
reg_t old = (insn.rd() != 0) ? p->get_csr(csr, insn, true) : 0; // don't if rd = 0
auto &carmap = p->get_state()->csrmap;
// permission on write
csrmap.count(csr) ? p->get_state()->csrmap[csr]->verify_permissions(insn, true) : throw trap_illegal_instruction(insn.bits());
p->put_csr(csr, insn.rs1());
WRITE_RD(sext_xlen(old));
serialize();
