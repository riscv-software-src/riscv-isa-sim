P_ONE_LOOP(8, {
  pd = 0;
  ps1 = ~ps1;
  if (!ps1) pd = 8;
  else {
    if ((ps1 & 0xF0) == 0) { pd += 4; ps1 <<= 4; }
    if ((ps1 & 0xC0) == 0) { pd += 2; ps1 <<= 2; }
    if ((ps1 & 0x80) == 0) { pd += 1; }
  }
})