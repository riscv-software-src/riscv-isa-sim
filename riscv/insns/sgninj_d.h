require_fp;
FRC.bits = (FRA.bits &~ INT64_MIN) | (FRB.bits & INT64_MIN);
