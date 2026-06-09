require_extension('P');
require_rv32;
uint32_t shamt = RS2 & 0x3f;
reg_t tmp = ((((uint128_t)P_RS1_PAIR << 1) >> shamt) + 1) >> 1;

if (tmp > UINT32_MAX) {
  WRITE_RD(sext32(UINT32_MAX));
  P.set_vxsat();
} else {
  WRITE_RD(sext32(tmp));
}

