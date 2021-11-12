//vslide1up.vx vd, vs2, rs1
VI_CHECK_SLIDE(true);

VI_LOOP_BASE
if (i != 0) {
  if (sew == e8) {
    VI_XI_SLIDEUP_PARAMS(e8, 1);
    vd = vs2;
  } else if(sew == e16) {
    VI_XI_SLIDEUP_PARAMS(e16, 1);
    vd = vs2;
  } else if(sew == e32) {
    VI_XI_SLIDEUP_PARAMS(e32, 1);
    vd = vs2;
  } else if(sew == e64) {
    VI_XI_SLIDEUP_PARAMS(e64, 1);
    vd = vs2;
  }
} else {
  if (sew == e8) {
    p->VU.elt<uint8_t>(rd_num, 0, true) = RS1;
  } else if(sew == e16) {
    p->VU.elt<uint16_t>(rd_num, 0, true) = RS1;
  } else if(sew == e32) {
    p->VU.elt<uint32_t>(rd_num, 0, true) = RS1;
  } else if(sew == e64) {
    p->VU.elt<uint64_t>(rd_num, 0, true) = RS1;
  }
}
VI_LOOP_END
