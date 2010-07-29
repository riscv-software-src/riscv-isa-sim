require_supervisor;

switch(insn.rtype.rb)
{
  case 0:
    set_sr(sext32(RA));
    break;
  case 1:
    epc = sext32(RA);
    break;
  case 3:
    ebase = sext32(RA & ~0xFFF);
    break;

  case 16:
    sim->set_tohost(sext32(RA));
    break;
}
