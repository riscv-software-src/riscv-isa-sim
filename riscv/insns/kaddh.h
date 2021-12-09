require_vector_vs;
require_extension(EXT_ZPN);
sreg_t res = (sreg_t)P_SH(RS1, 0) + (sreg_t)P_SH(RS2, 0);
P_SAT(res, 16);
WRITE_RD(sext_xlen((int16_t)res));
