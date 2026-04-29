require_extension('P');
require_rv32;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
if (sshamt < 0) {
  if (sshamt <= -32)
    WRITE_RD(0);
  else
    WRITE_RD(RS1 >> (-sshamt));
} else {
  uint64_t shx = (sshamt >= 32) ? ((uint64_t)RS1 << 32) : ((uint64_t)RS1 << sshamt);
  if (shx > 0xFFFFFFFFULL) {
    P.VU.vxsat->write(1);
    WRITE_RD(0xFFFFFFFF);
  } else {
    WRITE_RD((uint32_t)shx);
  }
}

