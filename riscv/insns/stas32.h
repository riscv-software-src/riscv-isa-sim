require_rv64;
P_STRAIGHT_LOOP(32, {
  pd = ps1 + ps2;
}, {
  pd = ps1 - ps2;
})