//vslide1band.vx vd, vs2, rs1
VI_CHECK_SLIDE(false);

uint16_t v_start = P.VU.vstart->read();

VI_LOOP_BASE

uint16_t band_i = i/16;
uint16_t flag_op = (RS1>>(band_i*4))&0xf; 
// 0x0: static
//
// 0x4: slide1up with 0x00 padding
// 0x5: slide1up with 0x01 padding
// 0x6: slide1up with 0x02 padding
// 0x7: slide1up with 0x03 padding
// 0x8: slide1up with 0xffff padding/
//
// 0x9: slide1down with 0x00 padding
// 0xa: slide1down with 0x01 padding
// 0xb: slide1down with 0x02 padding
// 0xc: slide1down with 0x03 padding
// 0xd: slide1down with 0xffff padding

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
  } else if(flag_op>=0x4 && flag_op<=0x8) {
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
        P.VU.elt<uint8_t>(rd_num, band_start, true) = (flag_op==0x8)? 0xff: (flag_op-0x4);
      } else if (sew==e16) {
        P.VU.elt<uint16_t>(rd_num, band_start, true) = (flag_op==0x8)? 0xffff: (flag_op-0x4);
      } else if (sew==e32) {
        P.VU.elt<uint32_t>(rd_num, band_start, true) = (flag_op==0x8)? 0xffffffff: (flag_op-0x4);
      } else if (sew==e64) {
        P.VU.elt<uint64_t>(rd_num, band_start, true) = (flag_op==0x8)? 0xffffffffffffffff: (flag_op-0x4);
      }
    }
  } else if(flag_op>=0x9 && flag_op<=0xd) {
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
        P.VU.elt<uint8_t>(rd_num, band_end, true) = (flag_op==0xd)? 0xff: (flag_op-0x9);
      } else if (sew==e16) {
        P.VU.elt<uint16_t>(rd_num, band_end, true) = (flag_op==0xd)? 0xffff: (flag_op-0x9);
      } else if (sew==e32) {
        P.VU.elt<uint32_t>(rd_num, band_end, true) = (flag_op==0xd)? 0xffffffff: (flag_op-0x9);
      } else if (sew==e64) {
          P.VU.elt<uint64_t>(rd_num, band_end, true) = (flag_op==0xd)? 0xffffffffffffffff: (flag_op-0x9);
      }
    }
  }
}

VI_LOOP_END
