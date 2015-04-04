require_extension('F');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(ui32_to_f32((uint32_t)RS1));
set_fp_exceptions;
