require_extension(EXT_ZICBOM);
require_envcfg(CBIE);

DECLARE_XENVCFG_VARS(CBIE);

if ((STATE.prv != PRV_M && mCBIE) ||
    (!STATE.v && STATE.prv == PRV_U && sCBIE) ||
    (STATE.v && (hCBIE || (STATE.prv == PRV_U && sCBIE))))
  MMU.clean_inval(RS1, true, true);
else
  MMU.clean_inval(RS1, false, true);
