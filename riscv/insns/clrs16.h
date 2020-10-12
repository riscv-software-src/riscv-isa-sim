P_ONE_LOOP(16, {
  pd = 0;
  if (ps1 < 0) ps1 = ~ps1;
  if (!ps1) pd = 16;
  else {
    if ((ps1 & 0xFF00) == 0) { pd += 8; ps1 <<= 8; }
    if ((ps1 & 0xF000) == 0) { pd += 4; ps1 <<= 4; }
    if ((ps1 & 0xC000) == 0) { pd += 2; ps1 <<= 2; }
    if ((ps1 & 0x8000) == 0) { pd += 1; }
  }
  pd -= 1;
})