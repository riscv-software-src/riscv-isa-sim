// vmpopc rd, vs2, vm
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
reg_t vl = p->VU.vl;
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();
require(p->VU.vstart == 0);
reg_t popcount = 0;
for (reg_t i=p->VU.vstart; i<vl; ++i){
  const int mlen = p->VU.vmlen;
  const int midx = (mlen * i) / 32;
  const int mpos = (mlen * i) % 32;

  bool vs2_lsb = ((p->VU.elt<uint32_t>(rs2_num, midx ) >> mpos) & 0x1) == 1;
  if (insn.v_vm() == 1) {
    popcount += vs2_lsb;
  } else {
    bool do_mask = (p->VU.elt<uint32_t>(0, midx) >> mpos) & 0x1;
    popcount += (vs2_lsb && do_mask);
  }
}
p->VU.vstart = 0;
WRITE_RD(popcount);
