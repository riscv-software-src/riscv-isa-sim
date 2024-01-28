#define COSIMIF
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

// bunlari yapabilmek icin sim_t'nin constructor'unu da degistirmek gerekecek.
// static void cosim_handle_signal(int sig)
// {
//   if (ctrlc_pressed)
//     exit(-1);
//   ctrlc_pressed = true;
//   signal(sig, &cosim_handle_signal);
// }

// bu asagidakini cfg degisiyor mu diye bakmak icin koymustum
// start_pc yanlis deger hatasi icin

// reg_t sim_t::htif_get_entry_point()
// {
//   return htif_t::get_entry_point();
// }

void sim_t::prerun()
{
  if (!debug && log)
    set_procs_debug(true);

  htif_t::set_expected_xlen(isa.get_max_xlen());
}

// void sim_t::htif_start()
// {
//   htif_t::start();
// }

// void sim_t::idle_single_step(){
//   std::cout << "sim_t::idle_single_step" << std::endl;
//   if (done())
//     return;

//   // once cosim_ctrlc_pressed kismini duzeltmem gerekiyor.
//   if (debug /*|| ctrlc_pressed*/)
//     interactive();
//   else{
//     step(1);
//   }

//   if (remote_bitbang)
//     remote_bitbang->tick();
//   // polimorfizm hatasi icin
//   // std::cout << "sim_t::idle_single_step" << std::endl;
// }

bool sim_t::communication_available()
{
  return htif_t::communication_available();
}

void sim_t::single_step_without_communication()
{
  std::cout << "sim_t::single_step_without_communication. object at: " << this << std::endl;
  htif_t::single_step_without_communication();
}

void sim_t::single_step_with_communication(std::queue<reg_t> *fromhost_queue, std::function<void(reg_t)> fromhost_callback)
{
  htif_t::single_step_with_communication(fromhost_queue, fromhost_callback);
}



