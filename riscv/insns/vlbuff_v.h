// vlbu.v and vlseg[2-8]bu.v
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
reg_t sew = p->VU.vsew;
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t rd_num = insn.rd();
bool early_stop = false;
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  STRIP(i)
  V_ELEMENT_SKIP(mmu_inx);

  uint64_t val = MMU.load_uint8(baseAddr + mmu_inx * 1);

  switch (sew) {
  case e8:
    p->VU.elt<uint8_t>(rd_num, i) = val;
    break;
  case e16:
    p->VU.elt<uint16_t>(rd_num, i) = val;
    break;
  case e32:
    p->VU.elt<uint32_t>(rd_num, i) = val;
    break;
  case e64:
    p->VU.elt<uint64_t>(rd_num, i) = val;
    break;
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
