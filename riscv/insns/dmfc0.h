require_supervisor;
require64;

switch(insn.rtype.rs)
{
  case 0:
    RT = sr;
    break;
  case 1:
    RT = epc;
    break;
  case 2:
    RT = badvaddr;
    break;
  case 3:
    RT = ebase;
    break;

  case 8:
    RT = MEMSIZE >> 12;
    break;

  case 17:
    RT = sim->get_fromhost();
    break;

  default:
    RT = -1;
}
