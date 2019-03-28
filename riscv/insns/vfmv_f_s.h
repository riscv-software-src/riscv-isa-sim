// vfmv_f_s: rd = vs2[0] (rs1=0)
require_fp;
require(STATE.VU.vsew == e8 || STATE.VU.vsew == e16 || STATE.VU.vsew == e32 || STATE.VU.vsew == e64); 

reg_t rs2_num = insn.rs2(); 

int32_t vs2 = STATE.VU.elt<int32_t>(rs2_num, 0); 
uint64_t sew_mask = (1 << STATE.VU.vsew) - 1;
WRITE_RD(vs2 & sew_mask);
