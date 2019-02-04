reg_t vl = STATE.VU.vl;
reg_t accVReg = insn.rd();
reg_t srcVReg = insn.rs2();
float a = f32(FRS1).v;
softfloat_roundingMode = RM;
for (reg_t i=STATE.VU.vstart; i<vl; ++i){
  float x = STATE.VU.elt<float>(srcVReg, i);
  float y = STATE.VU.elt<float>(accVReg, i);
  float r = f32_mulAdd({a}, {x}, {y}).v;
  STATE.VU.elt<float>(accVReg, i) = r;
  #if 0
  printf("vfmacc_vs(%lu) %f * %f + %f = %f\n", i, a, x, y, r);
  #endif
}
STATE.VU.vstart = 0;
