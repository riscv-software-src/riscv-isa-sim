#ifndef _RISCV_P_EXT_MACROS_H_
#define _RISCV_P_EXT_MACROS_H_

// rd temp
#define WRITE_P_RD() \
  rd_tmp = set_field(rd_tmp, make_mask64((i * sizeof(p_rd) * 8), sizeof(p_rd) * 8), p_rd);

// Field
#define P_FIELD(R, INDEX, SIZE) \
  (type_sew_t<SIZE>::type)get_field(R, make_mask64(((INDEX) * SIZE), SIZE))

// Params
#define P_RD_PARAMS(BIT) \
  auto p_rd = P_FIELD(rd_tmp, i, BIT);

// Loop base
#define P_RD_LOOP_BASE(BIT) \
  require_extension('P'); \
  require((BIT) == e8 || (BIT) == e16 || (BIT) == e32); \
  reg_t rd_tmp = RD; \
  sreg_t len = xlen / (BIT); \
  for (sreg_t i = len - 1; i >= 0; --i) {

// Loop body
#define P_RD_LOOP_BODY(BIT, BODY) { \
  P_RD_PARAMS(BIT) \
  BODY \
  WRITE_P_RD(); \
}

// Loop end
#define P_RD_LOOP_END() \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

// Loop
#define P_RD_LOOP(BIT_RD, BODY) \
  P_RD_LOOP_BASE(BIT_RD) \
  P_RD_LOOP_BODY(BIT_RD, BODY) \
  P_RD_LOOP_END()

#endif