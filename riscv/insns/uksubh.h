require_vector_vs;
require_extension(EXT_ZPN);
sreg_t res = (sreg_t)P_H(RS1, 0) - (sreg_t)P_H(RS2, 0);
P_SATU(res, 16);
WRITE_RD(sext_xlen((int16_t)res));
