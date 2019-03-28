// vfmv_s_f: vd[0] = rs1 (vs2=0)
require_fp;
require(STATE.VU.vsew == e8 || STATE.VU.vsew == e16 || STATE.VU.vsew == e32 || STATE.VU.vsew == e64); 

// SEW < FLEN ?
if (STATE.VU.vsew > FLEN){
// the least-significant FLEN bits are transferred and the upper SEW-FLEN bits are ignored.
    
}else if (STATE.VU.vsew < FLEN){
// the value is NaN-boxed (1-extended) to FLEN bits.
}


reg_t rd_num = insn.rd(); 
float32_t&vd = STATE.VU.elt<float32_t>(rd_num, 0); \
vd = f32(FRS1);
