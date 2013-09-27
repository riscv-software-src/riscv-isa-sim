require_fp;
WRITE_FRD(isNaNF32UI(FRS2) || f32_lt_quiet(FRS1,FRS2) /* && FRS1 not NaN */
      ? FRS1 : FRS2);
set_fp_exceptions;
