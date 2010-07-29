require_supervisor;
require64;

switch(insn.rtype.rb)
{
  case 0:
    set_sr(RA);
    break;
  case 1:
    epc = RA;
    break;
  case 3:
    ebase = RA & ~0xFFF;
    break;

  case 16:
    sim->set_tohost(RA);
    break;
}
