P_I_LOOP(16, 4, {
  if (imm4u > 0)
    pd = ((ps1 >> (imm4u - 1)) + 1) >> 1;
  else
    pd = ps1;
})