// vfwredsum.vs vd, vs2, vs1
switch (p->VU.fredsum_impl) {
  case processor_t::vectorUnit_t::ORDERED_FREDSUM_IMPL: { /* Ordered reduction sum */
    #include "vfwredosum_vs.h"
    }
    break;
  case processor_t::vectorUnit_t::PARALLEL_FREDSUM_IMPL: { /* Parallel reduction sum */
    #include "vfwredsum_vs_parallel.h"
    }
    break;
  default:
    require(0);
}
