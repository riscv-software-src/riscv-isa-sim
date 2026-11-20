require_extension('P');
require_rv32;
auto [tmp, sat] = (sat_addu<uint32_t>(RS1, RS2));
if (sat)
  P.set_vxsat();
 
WRITE_RD(sext32(tmp));
