// vwsmsac.vx  vd, vs2, rs1
V_WIDE_CHECK;
VRM vrm = STATE.VU.get_vround_mode();
const int gb = STATE.VU.vsew / 2;

#define WIDE_FUNC(sew1, sew2) \
  { \
    type_sew_t<sew2>::type &vd = STATE.VU.elt<type_sew_t<sew2>::type>(rd_num, i); \
    type_sew_t<sew1>::type rs1 = RS1; \
    type_sew_t<sew1>::type vs2 = STATE.VU.elt<type_sew_t<sew1>::type>(rs2_num, i); \
    int##sew2##_t round = (vrm == VRM::RUN) ? 1 << (gb - 1) : 0; \
    int##sew2##_t res; \
    bool sat = false; \
    res = ((((int##sew2##_t)(int##sew1##_t)vs2 * (int##sew2##_t)(int##sew1##_t)rs1) + round) >> gb); \
    vd = sat_add<int##sew2##_t, uint##sew2##_t>(vd, -res, sat); \
    STATE.VU.vxsat |= sat; \
  }
  
VI_LOOP_BASE
  V_LOOP_ELEMENT_SKIP;

  switch(STATE.VU.vsew) {
  case e8:
    WIDE_FUNC(8, 16);
    break;
  case e16:
    WIDE_FUNC(16, 32);
    break;
  default:
    WIDE_FUNC(32, 64);
    break;
  }

VI_LOOP_END
