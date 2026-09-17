require_extension(EXT_ZICBOM);
require_envcfg(CBIE);

DECLARE_XENVCFG_VARS(CBIE);

if ((STATE.prv != PRV_M && mCBIE == 1) ||
    (!STATE.v && STATE.prv == PRV_U && p->extension_enabled('S') && sCBIE == 1) ||
    (STATE.v && (hCBIE == 1 || (STATE.prv == PRV_U && sCBIE == 1))))
  MMU.clean_inval(RS1, true, true);
else
  MMU.clean_inval(RS1, false, true);
