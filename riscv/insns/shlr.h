require_extension('P');
require_rv64;
int sshamt = P_FIELD(RS2, 0, 8);
if (sshamt < 0) {
  uint128_t shx = ((uint128_t)RS1 << 1) >> std::min(-sshamt, 64);
  WRITE_RD((uint64_t)((shx + 1) >> 1));
} else {
  WRITE_RD((uint128_t)RS1 << std::min(sshamt, 64));
}

