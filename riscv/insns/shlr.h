require_extension('P');
require_rv64;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
if (sshamt < 0) {
  __uint128_t shx;
  if (sshamt < -64)
    shx = 0;
  else if (sshamt == -64)
    shx = (RS1 >> 63) & 1;
  else
    shx = ((__uint128_t)RS1 << 1) >> (-sshamt);
  WRITE_RD((uint64_t)((shx + 1) >> 1));
} else {
  if (sshamt >= 64)
    WRITE_RD(0);
  else
    WRITE_RD(RS1 << sshamt);
}

