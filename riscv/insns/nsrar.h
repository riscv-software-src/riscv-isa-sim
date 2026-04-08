require_extension('P');
require_rv32;
sreg_t val = (sreg_t)P_RS1_PAIR;
uint32_t shamt = RS2 & 0x3f;
sreg_t result;
if (shamt == 0) {
    result = val;
} else {
    result = (val >> shamt) + ((val >> (shamt - 1)) & 1);
}
WRITE_RD(result);
