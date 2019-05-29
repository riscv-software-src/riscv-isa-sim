// vlwff.v 
require(p->VU.vsew >= e32 && p->VU.vsew <= e64);
reg_t sew = p->VU.vsew;
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t rd_num = insn.rd();
bool early_stop = false;
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  STRIP(i)
  V_ELEMENT_SKIP(mmu_inx);

  int64_t val = MMU.load_int32(baseAddr + mmu_inx * 4);
  
  if (p->VU.vsew == e32) {
    p->VU.elt<uint32_t>(rd_num, i) = val;
  } else {
    p->VU.elt<uint64_t>(rd_num, i) = val;
  }

  if (val == 0) {
    p->VU.vl = i;
    early_stop = true;
    break;
  }
}

if (!early_stop) {
  VI_TAIL_ZERO(1);
}
p->VU.vstart = 0;
VI_CHECK_1905
