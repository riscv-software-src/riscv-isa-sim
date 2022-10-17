require_either_extension(EXT_ZFHMIN, EXT_ZHINXMIN);
require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_D(f16_to_f64(FRS1_H));
set_fp_exceptions;
