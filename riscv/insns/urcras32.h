require_rv64;
P_CROSS_ULOOP(32, {
  pd = ((uint64_t)ps1 + ps2) >> 1;
}, {
  pd = ((uint64_t)ps1 - ps2) >> 1;
})
