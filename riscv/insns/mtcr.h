reg_t val = gprlen == 64 ? RA : sext32(RA);

switch(insn.rtype.rb)
{
  case 29:
    tid = val;
    break;
}
