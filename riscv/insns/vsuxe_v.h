// vsuxe.v
reg_t sew = P.VU.vsew;
require(sew >= e8 && sew <= e64);
VI_DUPLICATE_VREG(insn.rs2(), P.VU.vlmax);
if (sew == e8) {
  VI_ST_INDEX(index[i], fn, uint8,  1, false);
} else if (sew == e16) {
  VI_ST_INDEX(index[i], fn, uint16, 2, false);
} else if (sew == e32) {
  VI_ST_INDEX(index[i], fn, uint32, 4, false);
} else if (sew == e64) {
  VI_ST_INDEX(index[i], fn, uint64, 8, false);
}
