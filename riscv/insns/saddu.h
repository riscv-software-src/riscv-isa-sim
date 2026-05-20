require_extension('P');
require_rv32;
bool sat = false;
reg_t tmp = sat_addu<uint32_t>(RS1, RS2, sat);
if (sat)
  P.set_vxsat();
WRITE_RD(sext32(tmp));
