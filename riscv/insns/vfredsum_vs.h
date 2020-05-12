// vfredsum: vd[0] =  sum( vs2[*] , vs1[0] )
bool is_propagate = true;
switch (p->VU.fredsum_impl) {
  case processor_t::vectorUnit_t::ORDERED_FREDSUM_IMPL: { /* Ordered reduction sum */
      VI_VFP_VV_LOOP_REDUCTION
      ({
        vd_0 = f16_add(vd_0, vs2);
      },
      {
        vd_0 = f32_add(vd_0, vs2);
      },
      {
        vd_0 = f64_add(vd_0, vs2);
      })
    }
    break;
  case processor_t::vectorUnit_t::PARALLEL_FREDSUM_IMPL: { /* Parallel reduction sum */
    #include "vfredsum_vs_parallel.h"
    }
    break;
  default:
    require(0);
}
