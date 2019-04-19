// vfmv_s_f: vd[0] = rs1 (vs2=0)
require_fp;
require(STATE.VU.vsew == e32);

reg_t rd_num = insn.rd(); 
if (FLEN == 64)
  STATE.VU.elt<uint32_t>(rd_num, 0) = f64(FRS1).v;
else
  STATE.VU.elt<uint32_t>(rd_num, 0) = f32(FRS1).v;
