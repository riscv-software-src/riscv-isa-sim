require_supervisor;
require64;

switch(insn.rtype.rs)
{
  case 0:
    set_sr(RT);
    break;
  case 1:
    epc = RT;
    break;
  case 3:
    ebase = RT & ~0xFFF;
    break;

  case 16:
    sim->set_tohost(RT);
    break;
}
