// vfmv_f_s: rd = vs2[0] (rs1=0)
VI_VFP_COMMON;

uint64_t vs2_0 = 0;
const reg_t sew = P.VU.vsew;
switch (sew) {
  case e16:
    vs2_0 = P.VU.elt<uint16_t>(rs2_num, 0);
    break;
  case e32:
    vs2_0 = P.VU.elt<uint32_t>(rs2_num, 0);
    break;
  case e64:
    vs2_0 = P.VU.elt<uint64_t>(rs2_num, 0);
    break;
  default:
    require(0);
    break;
}

// nan_extened
if (FLEN > sew) {
  vs2_0 = vs2_0 | (UINT64_MAX << sew);
}

if (FLEN == 64) {
  WRITE_FRD(f64(vs2_0));
} else {
  WRITE_FRD(f32(vs2_0));
}

P.VU.vstart->write(0);
