require_extension('P');
require_rv32;
reg_t tmp = (reg_t)P_RS1_PAIR >> insn.shamtd();
if (tmp > UINT32_MAX) {
  WRITE_RD(sext32(UINT32_MAX));
  P.set_vxsat();
} else {
  WRITE_RD(sext32(tmp));
}
