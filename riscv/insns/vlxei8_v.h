// vlxei8.v and vlxseg[2-8]ei8.v
VI_DUPLICATE_VREG(insn.rs2(), e8);

switch(P.VU.vsew) {
  case e8: {
      VI_LD_INDEX(index[i], fn, int8, true);
    }
    break;
  case e16: {
      VI_LD_INDEX(index[i], fn, int16, true);
    }
    break;
  case e32: {
      VI_LD_INDEX(index[i], fn, int32, true);
    }
    break;
  case e64: {
      VI_LD_INDEX(index[i], fn, int64, true);
    }
    break;
  default:
    require(0);
    break;
};
