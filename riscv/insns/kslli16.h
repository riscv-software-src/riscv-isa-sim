P_I_ULOOP(16, 4, {
  bool sat = false;
  pd = (sat_shl<int16_t, uint16_t>(ps1, imm4u, sat));
  P.VU.vxsat |= sat;
})