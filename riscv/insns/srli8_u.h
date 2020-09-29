P_I_ULOOP(8, 3, {
  if (imm3u > 0) {
    pd = ((ps1 >> (imm3u - 1)) + 1) >> 1;
  } else {
    pd = ps1;
  }
})