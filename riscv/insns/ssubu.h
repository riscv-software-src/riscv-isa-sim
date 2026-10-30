require_extension('P');
require_rv32;
auto [p_rd, sat] = (sat_subu<uint32_t>(RS1, RS2));
if (sat)
  P.set_vxsat();
WRITE_RD(sext32(p_rd));
