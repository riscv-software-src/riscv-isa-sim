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
    val = sim->get_fromhost();
    break;

  default:
    val = -1;
}

RA = gprlen == 64 ? val : sext32(val);
