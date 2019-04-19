// vfmv_f_s: rd = vs2[0] (rs1=0)
require_fp;
require(STATE.VU.vsew == e8 || STATE.VU.vsew == e16 || STATE.VU.vsew == e32 || STATE.VU.vsew == e64); 

reg_t rs2_num = insn.rs2(); 

int64_t vs2 = STATE.VU.elt<int32_t>(rs2_num, 0);
if (xlen == 64)
  WRITE_FRD(f64(vs2));
else
  WRITE_FRD(f32(vs2));
