// vrevband.vx  vd, vs2, rs1

VI_VI_LOOP
({
  uint16_t bw = 0;
  switch(RS1) {
    case 0: {
      bw = 16;
    }
    break;
    case 1: {
      bw = 32;
    }
    break;
    case 2: {
      bw = 64;
    }
    break;
    case 3: {
      bw = 128;
    }
    break;
    case 4: {
      bw = 256;
    }
    break;
  }

  uint16_t block_start = (i / bw) * bw;
  uint16_t offset = i % bw;
  uint16_t idx = block_start + bw - 1 - offset;

  auto &vd = P.VU.elt<uint16_t>(rd_num, i, true);
  auto vs2 = P.VU.elt<uint16_t>(rs2_num, idx);
  vd = vs2;
})


// #define VI_XI_SLIDEDOWN_PARAMS(x, off) \
//   auto &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
//   auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i + off);
