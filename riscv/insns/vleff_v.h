// vleff.v and vleseg[2-8]ff.v
reg_t sew = P.VU.vsew;

if (sew == e8) {
  VI_LDST_FF(int,  8, true);
} else if (sew == e16) {
  VI_LDST_FF(int, 16, true);
} else if (sew == e32) {
  VI_LDST_FF(int, 32, true);
} else if (sew == e64) {
  VI_LDST_FF(int, 64, true);
}
