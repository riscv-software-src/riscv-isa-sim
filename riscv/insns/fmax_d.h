require_fp;
WRITE_FRD(isNaNF64UI(FRS2) || f64_le_quiet(FRS2,FRS1) /* && FRS1 not NaN */
      ? FRS1 : FRS2);
set_fp_exceptions;
