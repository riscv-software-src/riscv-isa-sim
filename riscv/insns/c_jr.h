require_extension(EXT_ZCA);
require(insn.rvc_rs1() != 0);
set_pc(RVC_RS1 & ~reg_t(1));

maybe_set_elp(insn.rvc_rs1());
