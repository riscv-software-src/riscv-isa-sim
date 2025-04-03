require_extension(EXT_ZCMP);
require(insn.rvc_r1sc() != insn.rvc_r2sc());
WRITE_REG(RVC_R1S, READ_REG(X_A0));
WRITE_REG(RVC_R2S, READ_REG(X_A1));
