#ifdef BODY
require_extension(EXT_ZCA);
uint64_t rd = insn.citype.rvc_rd;
if (rd == 2) { // c.addi16sp
  require(RVC_ADDI16SP_IMM != 0);
  WRITE_REG(X_SP, sext_xlen(RVC_SP + RVC_ADDI16SP_IMM));
} else if (RVC_IMM != 0) { // c.lui
  WRITE_RD(RVC_IMM << 12);
} else if ((rd & 0x11) == 1) { // c.mop.N
  if (rd == 5 && p->extension_enabled(EXT_ZICFISS)) {
    #include "c_sspopchk_x5.h"
  } else if (rd == 1 && p->extension_enabled(EXT_ZICFISS)) {
    #include "c_sspush_x1.h"
  } else {
    #include "c_mop_N.h"
  }
} else {
  require(false);
}

#endif

#define CITYPE_INSN
