require_extension(EXT_ZICBOM);
DECLARE_XENVCFG_VARS(CBIE);
require_envcfg(CBIE);
if (((STATE.prv != PRV_M) && (mCBIE == 1)) ||
    ((!STATE.v && (STATE.prv == PRV_U)) && (sCBIE = 1)) ||
    (STATE.v && ((hCBIE == 1) || ((STATE.prv == PRV_U) && (sCBIE== 0)))))
  MMU.clean_inval(RS1, true, true);
else
  MMU.clean_inval(RS1, false, true);
