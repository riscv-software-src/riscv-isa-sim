require_extension('P');
require_rv32;
bool sat = false;
uint32_t p_rd = sat_subu<uint32_t>(RS1, RS2, sat);
if (sat)
  P.set_vxsat();
WRITE_RD(sext32(p_rd));
