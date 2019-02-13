reg_t vl = STATE.VU.vl;
reg_t accVReg = insn.rd();
reg_t srcVReg = insn.rs2();
float32_t a = f32(FRS1);
softfloat_roundingMode = STATE.frm;
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  float32_t x = STATE.VU.elt<float32_t>(srcVReg, i);
  float32_t y = STATE.VU.elt<float32_t>(accVReg, i);
  float32_t r = f32_mulAdd(a, x, y);
  STATE.VU.elt<float32_t>(accVReg, i) = r;
  #if 0
  printf("vfmacc_vs(%lu) %f * %f + %f = %f\n", i, to_f(a), to_f(x), to_f(y), to_f(r));
  #endif
}
STATE.VU.vstart = 0;
set_fp_exceptions;
