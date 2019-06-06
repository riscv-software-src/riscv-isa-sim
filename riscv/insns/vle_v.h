// vle.v and vlseg[2-8]e.v
reg_t sew = P.VU.vsew;

if (sew == e8) {
  VI_LD(0, (i * nf + fn), int8, 1);
} else if (sew == e16) {
  VI_LD(0, (i * nf + fn), int16, 2);
} else if (sew == e32) {
  VI_LD(0, (i * nf + fn), int32, 4);
} else if (sew == e64) {
  VI_LD(0, (i * nf + fn), int64, 8);
}

