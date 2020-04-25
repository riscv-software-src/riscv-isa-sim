// vlse.v and vlsseg[2-8]e.v
reg_t sew = P.VU.vsew;

if (sew == e8) {
  VI_LD(i * RS2, fn, int8, 1, true);
} else if (sew == e16) {
  VI_LD(i * RS2, fn, int16, 2, true);
} else if (sew == e32) {
  VI_LD(i * RS2, fn, int32, 4, true);
} else if (sew == e64) {
  VI_LD(i * RS2, fn, int64, 8, true);
}

