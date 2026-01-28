require_extension('P');
require_rv32;
sreg_t sshamt = P_FIELD(RS2, 0, 8);
if (RS1 == 0)
  WRITE_RD(0);
else if (sshamt >= 32)
  WRITE_RD((RS1 & 0x80000000) ? 0x80000000 : 0x7fffffff);
else if (sshamt <= -32)
  WRITE_RD(0);
else
  WRITE_RD(sshamt >= 0 ? P_SAT(32, static_cast<sreg_t> (RS1) << sshamt) : ((RS1 >> -sshamt) + ((RS1 >> (-sshamt - 1)) & 1)));