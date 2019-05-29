// vlwff.v 
require(p->VU.vsew >= e32 && p->VU.vsew <= e64);
reg_t sew = p->VU.vsew;
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t rd_num = insn.rd();
bool early_stop = false;
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  STRIP(i)
  V_ELEMENT_SKIP(i);

  int64_t val = MMU.load_int32(baseAddr + i * 4);
  
  if (p->VU.vsew == e32) {
    p->VU.elt<uint32_t>(rd_num, vreg_inx) = is_valid ? val : 0;
  } else {
    p->VU.elt<uint64_t>(rd_num, vreg_inx) = is_valid ? val : 0;
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
