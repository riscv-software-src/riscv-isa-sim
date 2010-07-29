require_supervisor;
uint32_t temp = sr;
set_sr(sr | SR_ET);
RA = temp;
