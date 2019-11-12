// vlxb.v and vlsseg[2-8]b.v
require(P.VU.vsew >= e8);
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);
VI_DUPLICATE_VREG(insn.rs2(), P.VU.vlmax);
VI_LD(index[i], fn, int8, 1);
