require_extension('P');
require_rv64;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
if (RS1 == 0)
  WRITE_RD(0);
else if (sshamt >= 64)
  WRITE_RD(0);
else if (sshamt <= -64)
  WRITE_RD((RS1 & 0x8000000000000000) ? 0xffffffffffffffff : 0);
else
  WRITE_RD(sshamt >= 0 ? (RS1 << sshamt) : ((sreg_t)RS1 >> -sshamt));
