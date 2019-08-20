// vsxe.v and vsxseg[2-8]e.v
reg_t sew = P.VU.vsew;
require(sew >= e8 && sew <= e64);
VI_DUPLICATE_VREG(insn.rs2(), P.VU.vlmax);
if (sew == e8) {
  VI_ST(index[i], fn, uint8, 1);
} else if (sew == e16) {
  VI_ST(index[i], fn, uint16, 2);
} else if (sew == e32) {
  VI_ST(index[i], fn, uint32, 4);
} else if (sew == e64) {
  VI_ST(index[i], fn, uint64, 8);
}

