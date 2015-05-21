require_extension('C');
reg_t tmp = npc;
set_pc(pc + insn.rvc_j_imm());
WRITE_REG(X_RA, tmp);
