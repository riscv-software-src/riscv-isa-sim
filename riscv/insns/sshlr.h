require_extension('P');
require_rv32;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
if (sshamt < 0) {
  uint64_t shx;
  if (sshamt < -32)
    shx = 0;
  else if (sshamt == -32)
    shx = (RS1 >> 31) & 1;
  else
    shx = ((uint64_t)RS1 << 1) >> (-sshamt);
  WRITE_RD((uint32_t)((shx + 1) >> 1));
} else {
  uint64_t shx = (sshamt >= 32) ? ((uint64_t)RS1 << 32) : ((uint64_t)RS1 << sshamt);
  if (shx > 0xFFFFFFFFULL) {
    P.VU.vxsat->write(1);
    WRITE_RD(0xFFFFFFFF);
  } else {
    WRITE_RD((uint32_t)shx);
  }
}

