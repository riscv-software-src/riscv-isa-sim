// vfmv_s_f: vd[0] = rs1 (vs2=0)
require_vector(true);
require_fp;
require((P.VU.vsew == e16 && p->supports_extension(EXT_ZFH)) ||
        (P.VU.vsew == e32 && p->supports_extension('F')) ||
        (P.VU.vsew == e64 && p->supports_extension('D')));
require(STATE.frm < 0x5);

reg_t vl = P.VU.vl;

if (vl > 0 && P.VU.vstart < vl) {
  reg_t rd_num = insn.rd();

  switch(P.VU.vsew) {
    case e16:
      P.VU.elt<uint16_t>(rd_num, 0, true) = f16(FRS1).v;
      break;
    case e32:
      P.VU.elt<uint32_t>(rd_num, 0, true) = f32(FRS1).v;
      break;
    case e64:
      if (FLEN == 64)
        P.VU.elt<uint64_t>(rd_num, 0, true) = f64(FRS1).v;
      else
        P.VU.elt<uint64_t>(rd_num, 0, true) = f32(FRS1).v;
      break;
  }
}
P.VU.vstart = 0;
