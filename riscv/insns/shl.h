require_extension('P');
require_rv64;
int sshamt = P_FIELD(RS2, 0, 8);
uint128_t shx = RS1;
if (sshamt < 0) {
  WRITE_RD(shx >> std::min(-sshamt, 64));
} else {
  WRITE_RD(shx << std::min(sshamt, 64));
}

