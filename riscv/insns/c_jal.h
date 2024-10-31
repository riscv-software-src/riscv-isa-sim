#ifdef BODY
require_extension(EXT_ZCA);
if (xlen == 32) {
  reg_t tmp = npc;
  set_pc(pc + RVC_J_IMM);
  WRITE_REG(X_RA, tmp);
} else { // c.addiw
  require(insn.cjtype.rvc_rd != 0);
  WRITE_RD(sext32(RVC_RS1 + RVC_IMM));
}

#endif

#define CJTYPE_INSN
