require_extension(EXT_ZCMP);
if (p->extension_enabled('E')) {
  require((insn.rvc_sreg1() < 2) && (insn.rvc_sreg2() < 2));
}
WRITE_REG(RVC_SREG1, READ_REG(X_A0));
WRITE_REG(RVC_SREG2, READ_REG(X_A1));
