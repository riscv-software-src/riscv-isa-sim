require_rvc;
if (RVC_RS1S != 0)
  set_pc(pc + insn.rvc_b_imm());
