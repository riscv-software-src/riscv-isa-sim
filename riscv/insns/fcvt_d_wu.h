require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_D(ui32_to_f64((uint32_t)RS1));
set_fp_exceptions;
