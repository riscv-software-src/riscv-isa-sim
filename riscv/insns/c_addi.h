require_rvc;
if(CRD_REGNUM == 0)
{
  reg_t temp = CRS1;
  if(CIMM6 & 0x20)
    RA = npc;
  set_pc(temp);
}
else
  CRD = sext_xprlen(CRS2 + CIMM6);
