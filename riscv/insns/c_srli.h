require_rvc;
if(xpr64)
  CRDS = CRDS >> CIMM5U;
else
  CRDS = sext32(uint32_t(CRDS) >> CIMM5U);
