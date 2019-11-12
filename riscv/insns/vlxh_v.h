// vlxh.v and vlxseg[2-8]h.v
require(P.VU.vsew >= e16);
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);
VI_DUPLICATE_VREG(insn.rs2(), P.VU.vlmax);
VI_LD(index[i], fn, int16, 2);
