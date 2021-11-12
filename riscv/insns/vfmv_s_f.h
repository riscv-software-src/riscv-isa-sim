// vfmv_s_f: vd[0] = rs1 (vs2=0)
require_vector(true);
require_fp;
require((p->VU.vsew == e16 && p->extension_enabled(EXT_ZFH)) ||
        (p->VU.vsew == e32 && p->extension_enabled('F')) ||
        (p->VU.vsew == e64 && p->extension_enabled('D')));
require(STATE.frm->read() < 0x5);

reg_t vl = p->VU.vl->read();

if (vl > 0 && p->VU.vstart->read() < vl) {
  reg_t rd_num = insn.rd();

  switch(p->VU.vsew) {
    case e16:
      p->VU.elt<uint16_t>(rd_num, 0, true) = f16(FRS1).v;
      break;
    case e32:
      p->VU.elt<uint32_t>(rd_num, 0, true) = f32(FRS1).v;
      break;
    case e64:
      if (FLEN == 64)
        p->VU.elt<uint64_t>(rd_num, 0, true) = f64(FRS1).v;
      else
        p->VU.elt<uint64_t>(rd_num, 0, true) = f32(FRS1).v;
      break;
  }
}
p->VU.vstart->write(0);
