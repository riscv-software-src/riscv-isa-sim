require_extension(EXT_ZICBOM);
DECLARE_XENVCFG_VARS(CBIE);
require_envcfg(CBIE);
if ((STATE.prv != PRV_M && mCBIE) ||
    (!STATE.v && STATE.prv == PRV_U && sCBIE) ||
    (STATE.v && (hCBIE || (STATE.prv == PRV_U && sCBIE))))
  MMU.clean_inval(RS1, true, true);
else
  MMU.clean_inval(RS1, false, true);
