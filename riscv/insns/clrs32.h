P_ONE_LOOP(32, {
  pd = 0;
  if (ps1 < 0) ps1 = ~ps1;
  if (!ps1) pd = 32;
  else {
    if ((ps1 & 0xFFFF0000) == 0) { pd += 16; ps1 <<= 16; }
    if ((ps1 & 0xFF000000) == 0) { pd += 8; ps1 <<= 8; }
    if ((ps1 & 0xF0000000) == 0) { pd += 4; ps1 <<= 4; }
    if ((ps1 & 0xC0000000) == 0) { pd += 2; ps1 <<= 2; }
    if ((ps1 & 0x80000000) == 0) { pd += 1; }
  }
  pd -= 1;
})