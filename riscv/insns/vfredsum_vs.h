// vfredsum: vd[0] =  sum( vs2[*] , vs1[0] )
switch (p->VU.fredsum_impl) {
  case processor_t::vectorUnit_t::ORDERED_FREDSUM_IMPL: { /* Ordered reduction sum */
    #include "vfredosum_vs.h"
    }
    break;
  case processor_t::vectorUnit_t::PARALLEL_FREDSUM_IMPL: { /* Parallel reduction sum */
    #include "vfredsum_vs_parallel.h"
    }
    break;
  default:
    require(0);
}
