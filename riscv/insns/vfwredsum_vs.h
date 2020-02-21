// vfwredsum.vs vd, vs2, vs1
if(p->VU.FREDSUM_IMPL == "ordered") {
  #include "vfwredosum_vs.h"
} else if (p->VU.FREDSUM_IMPL == "parallel") {
  #include "vfwredsum_vs_parallel.h"
} else
  require(0);
