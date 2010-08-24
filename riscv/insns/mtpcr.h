require_supervisor;

reg_t val = gprlen == 64 ? RA : sext32(RA);

switch(insn.rtype.rb)
{
  case 0:
    set_sr(val);
    break;
  case 1:
    epc = val;
    break;
  case 3:
    ebase = val & ~0xFFF;
    break;

  case 16:
    sim->set_tohost(val);
    break;
}
