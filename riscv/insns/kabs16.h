P_ONE_LOOP(16, {
  pd = ps1;
  if (ps1 == -32768) {
    pd = 32767;
  } else if (ps1 < 0) {
    pd = - ps1;
  }
})