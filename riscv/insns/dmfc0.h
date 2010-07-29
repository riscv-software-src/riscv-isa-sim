require_supervisor;
require64;

switch(insn.rtype.rb)
{
  case 0:
    RA = sr;
    break;
  case 1:
    RA = epc;
    break;
  case 2:
    RA = badvaddr;
    break;
  case 3:
    RA = ebase;
    break;

  case 8:
    RA = MEMSIZE >> 12;
    break;

  case 17:
    RA = sim->get_fromhost();
    break;

  default:
    RA = -1;
}
