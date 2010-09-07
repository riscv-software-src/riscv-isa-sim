reg_t val;

switch(insn.rtype.rb)
{
  case 1:
    val = 32; // synci_step
    break;

  case 29:
    val = tid;
    break;

  default:
    val = -1;
}

RC = gprlen == 64 ? val : sext32(val);
