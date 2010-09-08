require_supervisor;

reg_t val;

switch(insn.rtype.rb)
{
  case 0:
    val = sr;
    break;
  case 1:
    val = epc;
    break;
  case 2:
    val = badvaddr;
    break;
  case 3:
    val = ebase;
    break;

  case 8:
    val = MEMSIZE >> 12;
    break;

  case 17:
    fromhost = val = sim->get_fromhost();
    break;

  case 24:
    val = pcr_k0;
    break;
  case 25:
    val = pcr_k1;
    break;

  default:
    val = -1;
}

RC = gprlen == 64 ? val : sext32(val);
