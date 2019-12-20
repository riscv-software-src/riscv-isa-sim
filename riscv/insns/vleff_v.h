// vle.v and vlseg[2-8]e.v
reg_t sew = P.VU.vsew;

if (sew == e8) {
  VI_LDST_FF(int, 8);
} else if (sew == e16) {
  VI_LDST_FF(int, 16);
} else if (sew == e32) {
  VI_LDST_FF(int, 32);
} else if (sew == e64) {
  VI_LDST_FF(int, 64);
}
