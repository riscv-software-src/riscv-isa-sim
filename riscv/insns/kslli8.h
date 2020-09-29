P_I_ULOOP(8, 3, {
  bool sat = false;
  pd = (sat_shl<int8_t, uint8_t>(ps1, imm3u, sat));
  P.VU.vxsat |= sat;
})