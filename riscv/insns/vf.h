for (int i=0; i<VL; i++)
{
  uts[i]->pc = RS1+SIMM;
  uts[i]->utmode = true;
  while (uts[i]->utmode)
    uts[i]->step(n, noisy);
}
