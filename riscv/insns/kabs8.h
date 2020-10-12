P_ONE_LOOP(8, {
  pd = ps1;
  if (ps1 == -128) {
    pd = 127;
  } else if (ps1 < 0) {
    pd = - ps1;
  }
})