require_extension(EXT_ZCMP);
if (p->extension_enabled('E')) {
  require((insn.rvc_sreg1() < 2) && (insn.rvc_sreg2() < 2));
}
WRITE_REG(X_A0, READ_REG(RVC_SREG1));
WRITE_REG(X_A1, READ_REG(RVC_SREG2));
