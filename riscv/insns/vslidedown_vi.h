// vslidedown.vi vd, vs2, rs1
VI_LOOP_BASE
const reg_t sh = insn.v_zimm5();
bool is_valid = (i + sh) < P.VU.vlmax;
reg_t offset = 0;

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
