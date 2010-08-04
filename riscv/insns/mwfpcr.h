require_supervisor;

switch(insn.rtype.rb)
{
  case 0:
    RA = sext32(sr);
    break;
  case 1:
    RA = sext32(epc);
    break;
  case 2:
    RA = sext32(badvaddr);
    break;
  case 3:
    RA = sext32(ebase);
    break;

  case 8:
    RA = sext32(MEMSIZE >> 12);
    break;

  case 17:
    RA = sext32(sim->get_fromhost());
    break;

  default:
    RA = -1;
}
