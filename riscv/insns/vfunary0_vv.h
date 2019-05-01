// VFUNARY0 encoding space
VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    if (rs1_num == VFUNARY0::VFCVT_XU_F_V) {
      p->VU.elt<uint32_t>(rd_num, i) = f32_to_ui32(vs2, STATE.frm, true);
    } else if (rs1_num == VFUNARY0::VFCVT_X_F_V) {
      p->VU.elt<int32_t>(rd_num, i) = f32_to_i32(vs2, STATE.frm, true);
    } else if (rs1_num == VFUNARY0::VFCVT_F_XU_V) {
      auto vs2_u = p->VU.elt<uint32_t>(rs2_num, i);
      vd = ui32_to_f32(vs2_u);
    } else if (rs1_num == VFUNARY0::VFCVT_F_X_V) {
      auto vs2_i = p->VU.elt<int32_t>(rs2_num, i);
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
