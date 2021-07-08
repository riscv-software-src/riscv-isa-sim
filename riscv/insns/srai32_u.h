require_rv64;
P_I_LOOP(32, 5, {
  if (imm5u > 0)
    pd = (((uint64_t)(ps1 >> (imm5u - 1))) + 1) >> 1;
  else
    pd = ps1;
})