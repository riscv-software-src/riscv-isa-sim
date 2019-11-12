//vslidedown.vx vd, vs2, rs1
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);
require((insn.rd() & (P.VU.vlmul - 1)) == 0);
if (P.VU.vlmul > 1 && insn.v_vm() == 0)
  require(insn.rd() != 0);

const reg_t sh = RS1;
VI_LOOP_BASE

reg_t offset = 0;
bool is_valid = (i + sh) < P.VU.vlmax;

if (is_valid) {
  offset = sh;
}

switch (sew) {
case e8: {
  VI_XI_SLIDEDOWN_PARAMS(e8, offset);
  vd = is_valid ? vs2 : 0;
}
break;
case e16: {
  VI_XI_SLIDEDOWN_PARAMS(e16, offset);
  vd = is_valid ? vs2 : 0;
}
break;
case e32: {
  VI_XI_SLIDEDOWN_PARAMS(e32, offset);
  vd = is_valid ? vs2 : 0;
}
break;
default: {
  VI_XI_SLIDEDOWN_PARAMS(e64, offset);
  vd = is_valid ? vs2 : 0;
}
break;
}
VI_LOOP_END
