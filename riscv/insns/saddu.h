require_extension('P');
require_rv32;
bool sat = false;
WRITE_RD(sat_addu<uint32_t>(RS1, RS2, sat));