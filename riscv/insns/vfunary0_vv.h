// VFUNARY0 encoding space
switch (insn.rs1()) {
case VFUNARY0::VFWCVT_XU_F_V:
case VFUNARY0::VFWCVT_X_F_V:
case VFUNARY0::VFWCVT_F_XU_V:
case VFUNARY0::VFWCVT_F_X_V:
case VFUNARY0::VFWCVT_F_F_V:
  VI_WIDE_CHECK_ONE;
  break;
}

VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    //cvt
    switch (rs1_num) {
    case VFUNARY0::VFCVT_XU_F_V:
      p->VU.elt<uint32_t>(rd_num, i) = f32_to_ui32(vs2, STATE.frm, true);
      break;
    case VFUNARY0::VFCVT_X_F_V:
      p->VU.elt<int32_t>(rd_num, i) = f32_to_i32(vs2, STATE.frm, true);
      break;
    case VFUNARY0::VFCVT_F_XU_V: {
      auto vs2_u = p->VU.elt<uint32_t>(rs2_num, i);
      vd = ui32_to_f32(vs2_u);
      break;
    }
    case VFUNARY0::VFCVT_F_X_V: {
      auto vs2_i = p->VU.elt<int32_t>(rs2_num, i);
      vd = i32_to_f32(vs2_i);
      break;
    }

    //wcvt
    case VFUNARY0::VFWCVT_XU_F_V:
      p->VU.elt<uint64_t>(rd_num, i) = f32_to_ui64(vs2, STATE.frm, true);
      break;
    case VFUNARY0::VFWCVT_X_F_V:
      p->VU.elt<int64_t>(rd_num, i) = f32_to_i64(vs2, STATE.frm, true);
      break;
    case VFUNARY0::VFWCVT_F_XU_V: {
      auto vs2_u = p->VU.elt<uint32_t>(rs2_num, i);
      p->VU.elt<float64_t>(rd_num, i) = ui32_to_f64(vs2_u);
      break;
    }
    case VFUNARY0::VFWCVT_F_X_V: {
      auto vs2_i = p->VU.elt<int32_t>(rs2_num, i);
      p->VU.elt<float64_t>(rd_num, i) = i32_to_f64(vs2_i);
      break;
    }
    case VFUNARY0::VFWCVT_F_F_V: {
      auto vs2_f = p->VU.elt<float32_t>(rs2_num, i);
      p->VU.elt<float64_t>(rd_num, i) = f32_to_f64(vs2_f);
      break;
    }

    //ncvt
    case VFUNARY0::VFNCVT_XU_F_V: {
      auto vs2_d = p->VU.elt<float64_t>(rs2_num, i);
      p->VU.elt<uint32_t>(rd_num, i) = f64_to_ui32(vs2_d, STATE.frm, true);
      break;
    }
    case VFUNARY0::VFNCVT_X_F_V: {
      auto vs2_d = p->VU.elt<float64_t>(rs2_num, i);
      p->VU.elt<int32_t>(rd_num, i) = f64_to_i32(vs2_d, STATE.frm, true);
      break;
    }
    case VFUNARY0::VFNCVT_F_XU_V: {
      auto vs2_u = p->VU.elt<uint64_t>(rs2_num, i);
      vd = ui64_to_f32(vs2_u);
      break;
    }
    case VFUNARY0::VFNCVT_F_X_V: {
      auto vs2_i = p->VU.elt<int64_t>(rs2_num, i);
      vd = i64_to_f32(vs2_i);
      break;
    }
    case VFUNARY0::VFNCVT_F_F_V:
      auto vs2_d = p->VU.elt<float64_t>(rs2_num, i);
      p->VU.elt<float32_t>(rd_num, i) = f64_to_f32(vs2_d);
      break;
    }
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
