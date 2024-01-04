

#include "config.h"
#include "sim.h"
#include "mmu.h"
#include "dts.h"
#include "remote_bitbang.h"
#include "byteorder.h"
#include "platform.h"
#include "libfdt.h"
#include "socketif.h"
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void sim_t::pre_htif_run()
{
  if (!debug && log)
    set_procs_debug(true);

  htif_t::set_expected_xlen(isa.get_max_xlen());
}

void sim_t::htif_start()
{
  htif_t::start();
}

// !!! muhtemelen bunu yapmama gerek yoktu. 
// cunku commit'ler zaten execution'dan cikarken degil
// exectuion'a girerken temizleniyor. 
void sim_t::step_without_clear_commit(size_t n)
{
  for (size_t i = 0, steps = 0; i < n; i += steps)
  {
    steps = std::min(n - i, INTERLEAVE - current_step);
    procs[current_proc]->step_without_clear_commit(steps);

    current_step += steps;
    if (current_step == INTERLEAVE)
    {
      current_step = 0;
      procs[current_proc]->get_mmu()->yield_load_reservation();
      if (++current_proc == procs.size()) {
        current_proc = 0;
        reg_t rtc_ticks = INTERLEAVE / INSNS_PER_RTC_TICK;
        for (auto &dev : devices) dev->tick(rtc_ticks);
      }
    }
  }
}