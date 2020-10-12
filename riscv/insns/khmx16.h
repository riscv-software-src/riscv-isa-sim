P_CROSS_LOOP(16, {
  if ((ps1 != -32768) | (ps2 != -32768)) {
    pd = (ps1 * ps2) >> 15;
  } else {
    pd = 0x7FFF;
    P.VU.vxsat = 1;
  }
},)