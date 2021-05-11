P_CROSS_LOOP(16, {
  if ((ps1 != INT16_MIN) | (ps2 != INT16_MIN)) {
    pd = (ps1 * ps2) >> 15;
  } else {
    pd = INT16_MAX;
    P_SET_OV(1);
  }
},)