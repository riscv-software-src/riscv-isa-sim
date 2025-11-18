require_extension(EXT_ZCA);
reg_t tmp = npc;
set_pc(RVC_RS1 & ~reg_t(1));
WRITE_REG(X_RA, tmp);

maybe_set_elp(insn.rvc_rs1());
