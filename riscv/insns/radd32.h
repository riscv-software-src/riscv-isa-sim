require_rv64;
P_LOOP(32, {
  pd = ((int64_t)ps1 + ps2) >> 1;
})