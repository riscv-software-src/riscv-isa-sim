// vfmv_s_f: vd[0] = rs1 (vs2=0)
require_vector;
require_fp;
require_extension('F');
require(P.VU.vsew >= e32 && P.VU.vsew <= 64);
reg_t vl = P.VU.vl;

if (vl > 0) {
  reg_t rd_num = insn.rd();

  switch(P.VU.vsew) {
    case 32:
      if (FLEN == 64)
        P.VU.elt<uint32_t>(rd_num, 0, true) = f64(FRS1).v;
      else
        P.VU.elt<uint32_t>(rd_num, 0, true) = f32(FRS1).v;
      break;
    case 64:
      if (FLEN == 64)
        P.VU.elt<uint64_t>(rd_num, 0, true) = f64(FRS1).v;
      else
        P.VU.elt<uint64_t>(rd_num, 0, true) = f32(FRS1).v;
      break;
  }
}
P.VU.vstart = 0;
