require_fp;
uint32_t tmp = fsr;
set_fsr(RS1);
RD = tmp;
