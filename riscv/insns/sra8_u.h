P_X_LOOP(8, 3, {
  if(sa > 0)
    pd = ((ps1 >> (sa - 1)) + 1) >> 1;
  else
    pd = ps1;
})