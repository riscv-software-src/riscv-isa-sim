uint32_t nxpr = (XS1 & 0x3f) + (insn.i_imm() & 0x3f);
uint32_t nfpr = ((XS1 >> 6) & 0x3f) + ((insn.i_imm() >> 6) & 0x3f);
if (nxpr > 32)
  h->take_exception(HWACHA_CAUSE_ILLEGAL_CFG, 0);
if (nfpr > 32)
  h->take_exception(HWACHA_CAUSE_ILLEGAL_CFG, 1);
WRITE_NXPR(nxpr);
WRITE_NFPR(nfpr);
uint32_t maxvl;
if (nxpr + nfpr < 2)
  maxvl = 8 * 256;
else
  maxvl = 8 * (256 / (nxpr-1 + nfpr));
WRITE_MAXVL(maxvl);
WRITE_VL(0);
