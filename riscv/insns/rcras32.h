require_rv64;
P_CROSS_LOOP(32, {
  pd = ((int64_t)ps1 + ps2) >> 1;
}, {
  pd = ((int64_t)ps1 - ps2) >> 1;
})