require_supervisor;

switch(insn.rtype.rs)
{
  case 0:
    set_sr(sext32(RT));
    break;
  case 1:
    epc = sext32(RT);
    break;
  case 3:
    ebase = sext32(RT & ~0xFFF);
    break;

  case 16:
    sim->set_tohost(sext32(RT));
    break;
}
