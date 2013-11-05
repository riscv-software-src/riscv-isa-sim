require_fp;
WRITE_HFRD(isNaNF32UI(HFRS2) || f32_le_quiet(HFRS2,HFRS1) /* && FRS1 not NaN */
      ? HFRS1 : HFRS2);
set_fp_exceptions;
