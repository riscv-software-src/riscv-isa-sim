require_rv64;
P_X_ULOOP(32, 5, {
  if (sa > 0) {
    pd = (((uint64_t)(ps1 >> (sa - 1))) + 1) >> 1;
  } else {
    pd = ps1;
  }
})