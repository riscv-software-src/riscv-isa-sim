#ifndef _RISCV_P_EXT_MACROS_H_
#define _RISCV_P_EXT_MACROS_H_

// rd temp
#define WRITE_P_RD() \
  rd_tmp = set_field(rd_tmp, make_mask64((i * sizeof(p_rd) * 8), sizeof(p_rd) * 8), p_rd);

// Field
#define P_FIELD(R, INDEX, SIZE) \
  (type_sew_t<SIZE>::type)get_field(R, make_mask64(((INDEX) * SIZE), SIZE))

#define P_UFIELD(R, INDEX, SIZE) \
  (type_usew_t<SIZE>::type)get_field(R, make_mask64(((INDEX) * SIZE), SIZE))

// Params
#define P_RD_PARAMS(BIT) \
  auto p_rd = P_FIELD(rd_tmp, i, BIT);

#define P_RD_UPARAMS(BIT) \
  auto p_rd = P_UFIELD(rd_tmp, i, BIT);  

#define P_RS1_PARAMS(BIT) \
  auto p_rs1 = P_FIELD(rs1, i, BIT);

#define P_RS1_UPARAMS(BIT) \
  auto p_rs1 = P_UFIELD(rs1, i, BIT);

#define P_RS1_INNER_UPARAMS(BIT_INNER) \
  auto p_rs1 = P_UFIELD(rs1, j, BIT_INNER);

#define P_RS1_EVEN_PARAMS(BIT) \
  auto p_rs1 = P_FIELD(rs1, i * 2, BIT);

#define P_RS1_ODD_PARAMS(BIT) \
  auto p_rs1 = P_FIELD(rs1, i * 2 + 1, BIT);

#define P_RS1_EVEN_UPARAMS(BIT) \
  auto p_rs1 = P_UFIELD(rs1, i * 2, BIT);

#define P_RS1_ODD_UPARAMS(BIT) \
  auto p_rs1 = P_UFIELD(rs1, i * 2 + 1, BIT);

#define P_RS2_PARAMS(BIT) \
  auto p_rs2 = P_FIELD(rs2, i, BIT);  

#define P_RS2_UPARAMS(BIT) \
  auto p_rs2 = P_UFIELD(rs2, i, BIT);  

#define P_RS2_CROSS_PARAMS(BIT) \
  auto p_rs2 = P_FIELD(rs2, (i ^ 1), BIT); 
 
#define P_RS2_CROSS_UPARAMS(BIT) \
  auto p_rs2 = P_UFIELD(rs2, (i ^ 1), BIT);

#define P_RS2_INNER_UPARAMS(BIT_INNER) \
  auto p_rs2 = P_UFIELD(rs2, j, BIT_INNER);

#define P_RS2_EVEN_PARAMS(BIT) \
  auto p_rs2 = P_FIELD(rs2, i * 2, BIT);

#define P_RS2_ODD_PARAMS(BIT) \
  auto p_rs2 = P_FIELD(rs2, i * 2 + 1, BIT);  

#define P_RS2_EVEN_UPARAMS(BIT) \
  auto p_rs2 = P_UFIELD(rs2, i * 2, BIT);

#define P_RS2_ODD_UPARAMS(BIT) \
  auto p_rs2 = P_UFIELD(rs2, i * 2 + 1, BIT);

// Loop base
#define P_RD_LOOP_BASE(BIT) \
  require_extension('P'); \
  require((BIT) == e8 || (BIT) == e16 || (BIT) == e32); \
  reg_t rd_tmp = RD; \
  sreg_t len = xlen / (BIT); \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_RD_RS1_LOOP_BASE(BIT) \
  require_extension('P'); \
  require((BIT) == e8 || (BIT) == e16 || (BIT) == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  sreg_t len = xlen / (BIT); \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_RD_RS1_RS2_LOOP_BASE(BIT) \
  require_extension('P'); \
  require((BIT) == e8 || (BIT) == e16 || (BIT) == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  reg_t rs2 = RS2; \
  sreg_t len = xlen / (BIT); \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_RS1_LOOP_BASE(BIT) \
  require_extension('P'); \
  require((BIT) == e8 || (BIT) == e16 || (BIT) == e32); \
  reg_t rs1 = RS1; \
  sreg_t len = xlen / (BIT); \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_REDUCTION_ULOOP_BASE(BIT, BIT_INNER, USE_RD) \
  require_extension('P'); \
  require(BIT == e16 || BIT == e32 || BIT == e64); \
  reg_t rd_tmp = USE_RD ? zext_xlen(RD) : 0; \
  reg_t rs1 = zext_xlen(RS1); \
  reg_t rs2 = zext_xlen(RS2); \
  sreg_t len = 64 / BIT; \
  sreg_t len_inner = BIT / BIT_INNER; \
  for (sreg_t i = len - 1; i >= 0; --i) { \
    sreg_t p_res = P_UFIELD(rd_tmp, i, BIT); \
    for (sreg_t j = i * len_inner; j < (i + 1) * len_inner; ++j) {

// Loop body
#define P_RD_LOOP_BODY(BIT, BODY) { \
  P_RD_PARAMS(BIT) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_LOOP_BODY(BIT_RD, BIT_RS1, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_ULOOP_BODY(BIT_RD, BIT_RS1, BODY) { \
  P_RD_UPARAMS(BIT_RD) \
  P_RS1_UPARAMS(BIT_RS1) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  P_RS2_PARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_UPARAMS(BIT_RD) \
  P_RS1_UPARAMS(BIT_RS1) \
  P_RS2_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  P_RS2_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_CROSS_LOOP_BODY(BIT, BODY) { \
  P_RD_PARAMS(BIT) \
  P_RS1_PARAMS(BIT) \
  P_RS2_CROSS_PARAMS(BIT) \
  BODY \
  WRITE_P_RD(); \
}

#define P_CROSS_ULOOP_BODY(BIT, BODY) { \
  P_RD_UPARAMS(BIT) \
  P_RS1_UPARAMS(BIT) \
  P_RS2_CROSS_UPARAMS(BIT) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_EE_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_EVEN_PARAMS(BIT_RS1) \
  P_RS2_EVEN_PARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_EO_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_EVEN_PARAMS(BIT_RS1) \
  P_RS2_ODD_PARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_OO_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_ODD_PARAMS(BIT_RS1) \
  P_RS2_ODD_PARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_EE_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_UPARAMS(BIT_RD) \
  P_RS1_EVEN_UPARAMS(BIT_RS1) \
  P_RS2_EVEN_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_EO_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_UPARAMS(BIT_RD) \
  P_RS1_EVEN_UPARAMS(BIT_RS1) \
  P_RS2_ODD_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_OO_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_UPARAMS(BIT_RD) \
  P_RS1_ODD_UPARAMS(BIT_RS1) \
  P_RS2_ODD_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_EE_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_EVEN_PARAMS(BIT_RS1) \
  P_RS2_EVEN_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_OO_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_ODD_PARAMS(BIT_RS1) \
  P_RS2_ODD_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_E_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  P_RS2_EVEN_PARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_O_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  P_RS2_ODD_PARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_E_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  P_RS2_EVEN_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

#define P_RD_RS1_RS2_O_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) { \
  P_RD_PARAMS(BIT_RD) \
  P_RS1_PARAMS(BIT_RS1) \
  P_RS2_ODD_UPARAMS(BIT_RS2) \
  BODY \
  WRITE_P_RD(); \
}

// Loop end
#define P_RD_LOOP_END() \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_REDUCTION_ULOOP_END(BIT, IS_SAT) \
    } \
    if (IS_SAT) { \
      P_USAT(BIT, p_res); \
    } \
    type_usew_t<BIT>::type p_rd = p_res; \
    WRITE_P_RD(); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

// Loop
#define P_RD_LOOP(BIT_RD, BODY) \
  P_RD_LOOP_BASE(BIT_RD) \
  P_RD_LOOP_BODY(BIT_RD, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_LOOP(BIT_RD, BIT_RS1, BODY) \
  P_RD_RS1_LOOP_BASE(BIT_RD) \
  P_RD_RS1_LOOP_BODY(BIT_RD, BIT_RS1, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_ULOOP(BIT_RD, BIT_RS1, BODY) \
  P_RD_RS1_LOOP_BASE(BIT_RD) \
  P_RD_RS1_ULOOP_BODY(BIT_RD, BIT_RS1, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_LOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_ULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_SULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_CROSS_LOOP(BIT, BODY1, BODY2) \
  P_RD_RS1_RS2_LOOP_BASE(BIT) \
  P_CROSS_LOOP_BODY(BIT, BODY1) \
  --i; \
  if (sizeof(#BODY2) == 1) { \
    P_CROSS_LOOP_BODY(BIT, BODY1) \
  } \
  else { \
    P_CROSS_LOOP_BODY(BIT, BODY2) \
  } \
  P_RD_LOOP_END()

#define P_CROSS_ULOOP(BIT, BODY1, BODY2) \
  P_RD_RS1_RS2_LOOP_BASE(BIT) \
  P_CROSS_ULOOP_BODY(BIT, BODY1) \
  --i; \
  if (sizeof(#BODY2) == 1) { \
    P_CROSS_ULOOP_BODY(BIT, BODY1) \
  } \
  else { \
    P_CROSS_ULOOP_BODY(BIT, BODY2) \
  } \
  P_RD_LOOP_END()

#define P_REDUCTION_ULOOP(BIT, BIT_INNER, USE_RD, IS_SAT, BODY) \
  P_REDUCTION_ULOOP_BASE(BIT, BIT_INNER, USE_RD) \
  P_RS1_INNER_UPARAMS(BIT_INNER) \
  P_RS2_INNER_UPARAMS(BIT_INNER) \
  BODY \
  P_REDUCTION_ULOOP_END(BIT, IS_SAT)

#define P_RD_RS1_RS2_EE_LOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_EE_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_EO_LOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_EO_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_OO_LOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_OO_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_EE_ULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_EE_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_EO_ULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_EO_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_OO_ULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_OO_ULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_EE_SULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_EE_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_OO_SULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_OO_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_E_LOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_E_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_O_LOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_O_LOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_E_SULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_E_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

#define P_RD_RS1_RS2_O_SULOOP(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_RS1_RS2_LOOP_BASE(BIT_RD) \
  P_RD_RS1_RS2_O_SULOOP_BODY(BIT_RD, BIT_RS1, BIT_RS2, BODY) \
  P_RD_LOOP_END()

// Misc
#define P_SAT(BIT, R) ( \
  ((BIT) == 64) ? (R) : \
  ((R) > ((1LL << ((BIT) - 1)) - 1)) ? ((1LL << ((BIT) - 1)) - 1) : \
  ((R) < -(1LL << ((BIT) - 1))) ? -(1LL << ((BIT) - 1)) : \
  (R) \
)

#define P_USAT(BIT, R) ( \
  ((R) < 0) ? 0 : \
  ((BIT) == 64) ? (R) : \
  ((R) > ((1LL << ((BIT) - 1)) - 1)) ? ((1LL << ((BIT) - 1)) - 1) : \
  (R) \
)

#define P_PACK(BIT, X, Y) \
  require_extension('P'); \
  require(BIT == e8 || BIT == e16 || BIT == e32); \
  reg_t rd_tmp = 0, rs1 = RS1, rs2 = RS2; \
  for (sreg_t i = 0; i < xlen / BIT / 2; i++) { \
    rd_tmp = set_field(rd_tmp, make_mask64((i * 2 + 1) * BIT, BIT), \
      P_UFIELD(RS2, i * 2 + Y, BIT)); \
    rd_tmp = set_field(rd_tmp, make_mask64(i * 2 * BIT, BIT), \
      P_UFIELD(RS1, i * 2 + X, BIT)); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#endif