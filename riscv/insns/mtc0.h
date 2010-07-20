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
  case 8:
  {
    char ch = RT;
    demand(1 == write(1,&ch,1),"wtf");
    break;
  }
  case 9:
    printf("%ld insns retired\n",counters[0]);
    exit(0);
}
