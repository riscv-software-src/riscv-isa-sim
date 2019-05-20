// vmpopc rd, vs2, vm
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
reg_t vl = p->VU.vl;
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
require(p->VU.vstart == 0);

int cnt = 0;
if (vl > 0) {
  for (reg_t i = 0; i < vl; ++i) {
    const int mlen = p->VU.vmlen;
    const int midx = (mlen * i) / 64;
    const int mpos = (mlen * i) % 64;
    const uint32_t mmask = ((1ul << mlen) - 1) << mpos;

    bool vs2_lsb = ((p->VU.elt<uint64_t>(rs2_num, midx ) >> mpos) & 0x1) == 1;
    bool do_mask = (p->VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

    bool has_one = false;
    if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
      if (vs2_lsb) {
        has_one = true;
      }
    }

    bool use_ori = (insn.v_vm() == 0) && !do_mask;
    switch (sew) {
    case e8:
      P.VU.elt<uint8_t>(rd_num, i) = use_ori ?
        P.VU.elt<uint8_t>(rd_num, i) : cnt;
      break;
    case e16:
      P.VU.elt<uint16_t>(rd_num, i) = use_ori ?
        P.VU.elt<uint16_t>(rd_num, i) : cnt;
      break;
    case e32:
      P.VU.elt<uint32_t>(rd_num, i) = use_ori ?
        P.VU.elt<uint32_t>(rd_num, i) : cnt;
      break;
    default:
      P.VU.elt<uint64_t>(rd_num, i) = use_ori ?
        P.VU.elt<uint64_t>(rd_num, i) : cnt;
      break;
    }

    if (has_one) {
      cnt++;
    }
  }

  uint8_t *tail = &P.VU.elt<uint8_t>(rd_num, vl * ((sew >> 3) * 1)); \
  memset(tail, 0, (P.VU.vlmax - vl) * ((sew >> 3) * 1)); \
}
VI_CHECK_1905
