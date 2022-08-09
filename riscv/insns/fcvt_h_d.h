require_either_extension(EXT_ZFHMIN, EXT_ZHINXMIN);
require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_H(f64_to_f16(FRS1_D));
set_fp_exceptions;
