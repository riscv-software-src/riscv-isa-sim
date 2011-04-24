require_rvc;
if(CRD_REGNUM == 0)
{
  reg_t temp = npc;
  npc = CRS1;
  if(CIMM6 & 0x20)
    RA = temp;
}
else
  CRD = sext_xprlen(CRS2 + CIMM6);
