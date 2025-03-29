//vslide1band.vx vd, vs2, rs1
VI_CHECK_SLIDE(false);

uint16_t v_start = P.VU.vstart->read();

VI_LOOP_BASE

uint16_t band_i = i/16;
uint16_t flag_op = (RS1>>(band_i*4))&0x3; // 0: static, 1: slide1up, 2: slide1down

uint16_t band_start = band_i*16; 
uint16_t band_end = band_i*16+15;


if((i-v_start)<256){
  if (flag_op==0) {
      // Static operation
      // No need to slide
      if (sew == e8) { 
        VV_PARAMS(e8); 
        vd = vs2;
      } else if (sew == e16) { 
        VV_PARAMS(e16); 
        vd = vs2;
      } else if (sew == e32) { 
        VV_PARAMS(e32); 
        vd = vs2;
      } else if (sew == e64) { 
        VV_PARAMS(e64); 
        vd = vs2;
      }
  } else if(flag_op==1) {
    // Slide1up
    if (i != band_start) {
      if (sew==e8) {
          VI_XI_SLIDEUP_PARAMS(e8, 1);
          vd = vs2;
      } else if (sew==e16) {
          VI_XI_SLIDEUP_PARAMS(e16, 1);
          vd = vs2;
      } else if (sew==e32) {
          VI_XI_SLIDEUP_PARAMS(e32, 1);
          vd = vs2;
      } else if (sew==e64) {
          VI_XI_SLIDEUP_PARAMS(e64, 1);
          vd = vs2;
      }
    } else {
      if (sew==e8) {
        P.VU.elt<uint8_t>(rd_num, band_start, true) = 0xff;
      } else if (sew==e16) {
        P.VU.elt<uint16_t>(rd_num, band_start, true) = 0xffff;
      } else if (sew==e32) {
        P.VU.elt<uint32_t>(rd_num, band_start, true) = 0xffffffff;
      } else if (sew==e64) {
        P.VU.elt<uint64_t>(rd_num, band_start, true) = 0xffffffffffffffff;
      }
    }
  } else if(flag_op==2) {
    // Slide1down
    if (i!=band_end) {
      if (sew==e8) {
          VI_XI_SLIDEDOWN_PARAMS(e8, 1);
          vd = vs2;
      } else if (sew==e16) {
          VI_XI_SLIDEDOWN_PARAMS(e16, 1);
          vd = vs2;
      } else if (sew==e32) {
          VI_XI_SLIDEDOWN_PARAMS(e32, 1);
          vd = vs2;
      } else if (sew==e64) {
          VI_XI_SLIDEDOWN_PARAMS(e64, 1);
          vd = vs2;
      }
    } else {
      if (sew==e8) {
        P.VU.elt<uint8_t>(rd_num, band_end, true) = 0xff;
      } else if (sew==e16) {
        P.VU.elt<uint16_t>(rd_num, band_end, true) = 0xffff;
      } else if (sew==e32) {
        P.VU.elt<uint32_t>(rd_num, band_end, true) = 0xffffffff;
      } else if (sew==e64) {
          P.VU.elt<uint64_t>(rd_num, band_end, true) = 0xffffffffffffffff;
      }
    }
  }
}

VI_LOOP_END
