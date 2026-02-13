require_extension('P');
require_rv32;
reg_t tmp = (reg_t)P_RS1_PAIR >> insn.shamtd();
uint32_t result = P_USAT(32, tmp);
WRITE_RD(result);