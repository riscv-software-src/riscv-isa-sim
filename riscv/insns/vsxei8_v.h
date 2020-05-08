// vsxei8.v and vsxseg[2-8]ei8.v
VI_DUPLICATE_VREG(insn.rs2(), 8);

switch(P.VU.vsew) {
  case e8: {
      VI_ST_INDEX(index[i], fn, uint8, true);
    }
    break;
  case e16: {
      VI_ST_INDEX(index[i], fn, uint16, true);
    }
    break;
  case e32: {
      VI_ST_INDEX(index[i], fn, uint32, true);
    }
    break;
  case e64: {
      VI_ST_INDEX(index[i], fn, uint64, true);
    }
    break;
  default:
    require(0);
    break;
};
