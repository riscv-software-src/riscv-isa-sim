// vsuxe64.v
VI_DUPLICATE_VREG(insn.rs2(), 64);

switch(P.VU.vsew) {
  case e8: {
      VI_ST_INDEX(index[i], fn, uint8, false);
    }
    break;
  case e16: {
      VI_ST_INDEX(index[i], fn, uint16, false);
    }
    break;
  case e32: {
      VI_ST_INDEX(index[i], fn, uint32, false);
    }
    break;
  case e64: {
      VI_ST_INDEX(index[i], fn, uint64, false);
    }
    break;
  default:
    require(0);
    break;
};
