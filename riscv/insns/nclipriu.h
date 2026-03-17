require_extension('P');
require_rv32;
reg_t val = (reg_t)P_RS1_PAIR;
uint32_t shamt = insn.shamtd();
reg_t result;
if (shamt == 0) {
    result = P_USAT(32, val);
} else {
    result = P_USAT(32, (val >> shamt) + ((val >> (shamt - 1)) & 1));
}
WRITE_RD(result);