// vsse.v and vssseg[2-8]e.v
reg_t sew = P.VU.vsew;

if (sew == e8) {
  VI_ST(i * RS2, fn, uint8, 1);
} else if (sew == e16) {
  VI_ST(i * RS2, fn, uint16, 2);
} else if (sew == e32) {
  VI_ST(i * RS2, fn, uint32, 4);
} else if (sew == e64) {
  VI_ST(i * RS2, fn, uint64, 8);
}

