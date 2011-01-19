reg_t val;

switch(insn.rtype.rs2)
{
  case 0:
    require_fp;
    val = fsr;
    break;

  case 1:
    val = 32; // synci_step
    break;

  default:
    val = -1;
}

RD = sext_xprlen(val);
