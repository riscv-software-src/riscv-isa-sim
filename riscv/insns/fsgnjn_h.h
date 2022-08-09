require_either_extension(EXT_ZFH, EXT_ZHINX);
require_fp;
WRITE_FRD_H(fsgnj16(freg(FRS1_H), freg(FRS2_H), true, false));