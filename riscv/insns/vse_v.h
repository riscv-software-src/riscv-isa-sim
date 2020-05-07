// vse.v and vsseg[2-8]e.v
reg_t sew = P.VU.vsew;

const reg_t mew = insn.v_mew(); 
const reg_t width = insn.v_width(); 
VI_EEW(mew, width); 

if (P.VU.veew == e8) {
  VI_ST(0, (i * nf + fn), uint8,  1, true);
} else if (sew == e16) {
  VI_ST(0, (i * nf + fn), uint16, 2, true);
} else if (sew == e32) {
  VI_ST(0, (i * nf + fn), uint32, 4, true);
} else if (sew == e64) {
  VI_ST(0, (i * nf + fn), uint64, 8, true);
}

