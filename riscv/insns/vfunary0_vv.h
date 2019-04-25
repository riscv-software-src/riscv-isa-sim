// VFUNARY0 encoding space
VFP_VV_LOOP
({
  switch(STATE.VU.vsew){
  case e32:
    fprintf(stderr, "here : %lx %lx %lx\n", rd_num, rs1_num, rs2_num);
    if (rs1_num == VFUNARY0::VFCVT_XU_F_V) {
      STATE.VU.elt<uint32_t>(rd_num, i) = f32_to_ui32(vs2, STATE.frm, true);
    } else if (rs1_num == VFUNARY0::VFCVT_X_F_V) {
      STATE.VU.elt<int32_t>(rd_num, i) = f32_to_i32(vs2, STATE.frm, true);
    } else if (rs1_num == VFUNARY0::VFCVT_F_XU_V) {
      fprintf(stderr, "here : %lx\n", rs2_num);
      auto vs2_u = STATE.VU.elt<uint32_t>(rs2_num, i);
      vd = ui32_to_f32(vs2_u);
    } else if (rs1_num == VFUNARY0::VFCVT_F_X_V) {
      auto vs2_i = STATE.VU.elt<int32_t>(rs2_num, i);
      vd = i32_to_f32(vs2_i);
    } else if (rs1_num == VFUNARY0::VFWCVT_XU_F_V) {
    } else if (rs1_num == VFUNARY0::VFWCVT_X_F_V) {
    } else if (rs1_num == VFUNARY0::VFWCVT_F_XU_V) {
    } else if (rs1_num == VFUNARY0::VFWCVT_F_X_V) {
    } else if (rs1_num == VFUNARY0::VFWCVT_F_F_V) {
    } else if (rs1_num == VFUNARY0::VFNCVT_XU_F_V) {
    } else if (rs1_num == VFUNARY0::VFNCVT_X_F_V) {
    } else if (rs1_num == VFUNARY0::VFNCVT_F_XU_V) {
    } else if (rs1_num == VFUNARY0::VFNCVT_F_X_V) {
    } else if (rs1_num == VFUNARY0::VFNCVT_F_F_V) {
    }
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
