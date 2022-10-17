require_either_extension('D', EXT_ZDINX);
require_fp;
WRITE_FRD_D(fsgnj64(freg(FRS1_D), freg(FRS2_D), false, false));
