// vfmv_f_s: rd = vs2[0] (rs1=0)
VI_VFP_COMMON;
require(FLEN >= P.VU.vsew);

freg_t elt;

switch (P.VU.vsew) {
  case e16:
    elt = freg(P.VU.elt<float16_t>(rs2_num, 0));
    break;
  case e32:
    elt = freg(P.VU.elt<float32_t>(rs2_num, 0));
    break;
  case e64:
    elt = freg(P.VU.elt<float64_t>(rs2_num, 0));
    break;
  default:
    require(0);
    break;
}

WRITE_FRD(elt);

VECTOR_END;
