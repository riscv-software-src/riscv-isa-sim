// vfredsum: vd[0] =  sum( vs2[*] , vs1[0] )

if(p->VU.FREDSUM_IMPL == "ordered") {
  #include "vfredosum_vs.h"
} else if (p->VU.FREDSUM_IMPL == "parallel") {
  #include "vfredsum_vs_parallel.h"
} else
  require(0);
