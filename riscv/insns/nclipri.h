require_extension('P');
require_rv32;
sreg_t val = (sreg_t)P_RS1_PAIR;
uint32_t shamt = insn.shamtd();
sreg_t result;
if (shamt == 0) {
    result = P_SAT(32, val);
} else {
    result = P_SAT(32, (val >> shamt) + ((val >> (shamt - 1)) & 1));
}
WRITE_RD(result);