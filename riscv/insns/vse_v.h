// vsw.v and vsseg[2-8]w.v
reg_t sew = P.VU.vsew;

if (sew == e8) {
  VI_ST(0, (i * nf + fn), uint8, 1);
} else if (sew == e16) {
  VI_ST(0, (i * nf + fn), uint16, 2);
} else if (sew == e32) {
  VI_ST(0, (i * nf + fn), uint32, 4);
} else if (sew == e64) {
  VI_ST(0, (i * nf + fn), uint64, 8);
}

