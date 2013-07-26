require_rvc;
if(xpr64)
  CRDS = sreg_t(CRDS) >> CIMM5U;
else
  CRDS = sext32(int32_t(CRDS) >> CIMM5U);
