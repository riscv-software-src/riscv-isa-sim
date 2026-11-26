require_extension('P');
require_rv32;
int sshamt = P_FIELD(RS2, 0, 8);
uint64_t shx;
if (sshamt < 0) {
  shx = ((uint64_t)(uint32_t)RS1 << 1) >> std::min(-sshamt, 32);
  shx = (shx + 1) >> 1;
} else {
  shx = (uint64_t)(uint32_t)RS1 << std::min(sshamt, 32);
  if (shx > UINT32_MAX) {
    P.set_vxsat();
    shx = UINT32_MAX;
  }
}

WRITE_RD(sext32(shx));
