P_LOOP(8, {
  if ((ps1 != -128) | (ps2 != -128)) {
    pd = (ps1 * ps2) >> 7;
  } else {
    pd = 0x7F;
    P.VU.vxsat = 1;
  }
})