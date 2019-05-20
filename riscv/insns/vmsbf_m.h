// vmsbf rd.m, vs2, vm
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
reg_t vl = p->VU.vl;
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
require(p->VU.vstart == 0);

bool has_one = false;
for (reg_t i = 0 ; i < P.VU.vlmax; ++i) {
  const int mlen = p->VU.vmlen;
  const int midx = (mlen * i) / 32;
  const int mpos = (mlen * i) % 32;
  const uint32_t mmask = ((1ul << mlen) - 1) << mpos;

  bool vs2_lsb = ((p->VU.elt<uint32_t>(rs2_num, midx ) >> mpos) & 0x1) == 1;
  bool do_mask = (p->VU.elt<uint32_t>(0, midx) >> mpos) & 0x1;
  uint32_t &vd = P.VU.elt<uint32_t>(rd_num, midx);

  if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
    int res = 0;
    if (!has_one && !vs2_lsb) {
      res = 1;
    } else if(vs2_lsb) {
      has_one = true;
    }
    vd = (vd & ~mmask) | ((res << mpos) & mmask);
  }
}
