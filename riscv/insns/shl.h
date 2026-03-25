require_extension('P');
require_rv64;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
if (sshamt < 0) {
  if (sshamt <= -64)
    WRITE_RD(0);
  else
    WRITE_RD(RS1 >> (-sshamt));
} else {
  if (sshamt >= 64)
    WRITE_RD(0);
  else
    WRITE_RD(RS1 << sshamt);
}

