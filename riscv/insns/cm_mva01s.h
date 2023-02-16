require_extension(EXT_ZCMP);
if (p->extension_enabled('E')) {
  require((insn.rvc_r1sc() < 2) && (insn.rvc_r2sc() < 2));
}
WRITE_REG(X_A0, READ_REG(RVC_R1S));
WRITE_REG(X_A1, READ_REG(RVC_R2S));
