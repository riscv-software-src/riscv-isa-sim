// vmsbf.m vd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector;
reg_t vl = P.VU.vl;
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();

bool has_one = false;
for (reg_t i = P.VU.vstart; i < vl; ++i) {
  const int mlen = P.VU.vmlen;
  const int midx = (mlen * i) / 64;
  const int mpos = (mlen * i) % 64;
  const uint64_t mmask = (UINT64_MAX << (64 - mlen)) >> (64 - mlen - mpos);

  bool vs2_lsb = ((P.VU.elt<uint64_t>(rs2_num, midx) >> mpos) & 0x1) == 1;
  bool do_mask = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;
  auto &vd = P.VU.elt<uint64_t>(rd_num, midx);


  if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
    uint64_t res = 0;
    if (!has_one && !vs2_lsb) {
      res = 1;
    } else if(!has_one && vs2_lsb) {
      has_one = true;
    }
    vd = (vd & ~mmask) | ((res << mpos) & mmask);
  }
}

VI_TAIL_ZERO_MASK(rd_num);
P.VU.vstart = 0;
