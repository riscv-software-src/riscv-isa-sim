require_extension(EXT_ZCA);
if (insn.rvc_rd() == 2) { // c.addi16sp
  require(insn.rvc_addi16sp_imm() != 0);
  WRITE_REG(X_SP, sext_xlen(RVC_SP + insn.rvc_addi16sp_imm()));
} else if (insn.rvc_imm() != 0) { // c.lui
  WRITE_RD(insn.rvc_imm() << 12);
} else if ((insn.rvc_rd() & 0x11) == 1) { // c.mop.N
  if (insn.rvc_rd() == 5 && p->extension_enabled(EXT_ZICFISS)) {
    #include "c_sspopchk_x5.h"
  } else if (insn.rvc_rd() == 1 && p->extension_enabled(EXT_ZICFISS)) {
    #include "c_sspush_x1.h"
  } else {
    #include "c_mop_N.h"
  }
} else {
  require(false);
}
