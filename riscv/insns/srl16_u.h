P_X_ULOOP(16, 4, {
  if (sa > 0) {
    pd = ((ps1 >> (sa - 1)) + 1) >> 1;
  } else {
    pd = ps1;
  }
})
