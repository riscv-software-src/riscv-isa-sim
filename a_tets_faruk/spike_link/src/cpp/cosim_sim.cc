

#include "config.h"
#define COSIMIF
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

// bunlari yapabilmek icin sim_t'nin constructor'unu da degistirmek gerekecek.
// static void cosim_handle_signal(int sig)
// {
//   if (ctrlc_pressed)
//     exit(-1);
//   ctrlc_pressed = true;
//   signal(sig, &cosim_handle_signal);
// }


void sim_t::htif_prerun()
{
  if (!debug && log)
    set_procs_debug(true);

  htif_t::set_expected_xlen(isa.get_max_xlen());
}

void sim_t::htif_start()
{
  htif_t::start();
}

void sim_t::idle_single_step(){
  if (done())
    return;

  // once cosim_ctrlc_pressed kismini duzeltmem gerekiyor.
  if (debug /*|| ctrlc_pressed*/)
    interactive();
  else{
    step(1);
  }

  if (remote_bitbang)
    remote_bitbang->tick();
}

bool sim_t::htif_communication_available()
{
  return htif_t::communication_available();
}

void sim_t::htif_single_step_without_communication()
{
  htif_t::single_step_without_communication();
}

void sim_t::htif_single_step_with_communication(std::queue<reg_t> *fromhost_queue, std::function<void(reg_t)> fromhost_callback)
{
  htif_t::single_step_with_communication(fromhost_queue, fromhost_callback);
}



