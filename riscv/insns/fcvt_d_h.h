require_extension(EXT_ZFHMIN);
require_extension('D');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f16_to_f64(f16(FRS1)));
set_fp_exceptions;
