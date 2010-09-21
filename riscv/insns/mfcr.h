reg_t val;

switch(insn.rtype.rs2)
{
  case 0:
    val = fsr;
    break;

  case 1:
    val = 32; // synci_step
    break;

  case 29:
    val = tid;
    break;

  default:
    val = -1;
}

RDR = gprlen == 64 ? val : sext32(val);
