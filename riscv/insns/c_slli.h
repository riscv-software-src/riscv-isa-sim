require_rvc;
if(xpr64)
  CRDS = CRDS << CIMM5U;
else
  CRDS = sext32(CRDS << CIMM5U);
