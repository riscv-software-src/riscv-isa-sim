require_extension(EXT_ZFHMIN);
require_extension('D');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f64_to_f16(f64(FRS1)));
set_fp_exceptions;
