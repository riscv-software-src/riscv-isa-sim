P_CROSS_LOOP(8, {
  if ((ps1 != INT8_MIN) | (ps2 != INT8_MIN)) {
    pd = (ps1 * ps2) >> 7;
  } else {
    pd = INT8_MAX;
    P_SET_OV(1);
  }
},)