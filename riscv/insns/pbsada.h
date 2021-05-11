P_REDUCTION_ULOOP(64, 8, true, false, {
  pd_res += (ps1 > ps2 ? ps1 - ps2 : ps2 - ps1);
})