require_rv64;
P_CROSS_LOOP(32, {
  pd = ps1 + ps2;
}, {
  pd = ps1 - ps2;
})