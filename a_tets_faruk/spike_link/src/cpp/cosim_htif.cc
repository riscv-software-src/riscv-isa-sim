// See LICENSE for license details.

#define COSIMIF
#include "config.h"
#include "htif.h"
#include "rfb.h"
#include "elfloader.h"
#include "platform.h"
#include "byteorder.h"
#include "trap.h"
#include "../riscv/common.h"
#include <algorithm>
#include <assert.h>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>



// bunlari yapabilmek icin htif_t'nin constructor'unu da degistirmek gerekecek.
// static volatile bool cosim_signal_exit = false;
// static void cosim_handle_signal(int sig)
// {
//   if (sig == SIGABRT || cosim_signal_exit) // someone set up us the bomb!
//     exit(-1);
//   cosim_signal_exit = true;
//   signal(sig, &cosim_handle_signal);
// }


static void bad_address(const std::string& situation, reg_t addr)
{
  std::cerr << "Access exception occurred while " << situation << ":\n";
  std::cerr << "Memory address 0x" << std::hex << addr << " is invalid\n";
  exit(-1);
}

bool htif_t::communication_available()
{
  return tohost_addr != 0;
}



void htif_t::single_step_with_communication(std::queue<reg_t> *fromhost_queue, std::function<void(reg_t)> fromhost_callback)
{
  uint64_t tohost;

    // !!! to host diye bir mesaj okunup sifirlaniyor
    try {
    // tohost, printf kismi.
      if ((tohost = from_target(mem.read_uint64(tohost_addr))) != 0)
        mem.write_uint64(tohost_addr, target_endian<uint64_t>::zero);
    } catch (mem_trap_t& t) {
      bad_address("accessing tohost", t.get_tval());
    }

    try {
      if (tohost != 0) {
        // std::cout << "faruk: tohost: " << tohost << std::endl;
        command_t cmd(mem, tohost, fromhost_callback);
        device_list.handle_command(cmd);
      } else {
        idle_single_step();
      }

      device_list.tick();
    } catch (mem_trap_t& t) {
      std::stringstream tohost_hex;
      tohost_hex << std::hex << tohost;
      // !!! ben tohost'a rastgele bir sey yazdigimda, 
      // device.cc'de handle command'de syscall_proxy'ye giden
      // command'de, syscall'i bulamiyor. cmd.device() = syscall_proxy
      // 
      
      bad_address("host was accessing memory on behalf of target (tohost = 0x" + tohost_hex.str() + ")", t.get_tval());
    }

    try {
      if (!(fromhost_queue->empty()) && !mem.read_uint64(fromhost_addr)) {
        // std::cout << "faruk: fromhost: " << fromhost_queue.front() << std::endl;
        mem.write_uint64(fromhost_addr, to_target(fromhost_queue->front()));
        fromhost_queue->pop();
      }
    } catch (mem_trap_t& t) {
      // !!!  burda fromhost, tohost tanimlamadan bir elf dosyasi olusturup 
      // !!! sonra debug modda core'umuza bir seyler gondererek? bu hatayi almayi deneyebilirim
      bad_address("accessing fromhost", t.get_tval());
    }
}

void htif_t::single_step_without_communication()
{
  //polimorfizm hatasi icin
  // idle();
  #if DEBUG_LEVEL >= DEBUG_WARN
  std::cout << "htif_t::single_step_without_communication" 
  " object at "<< this << " calling idle_single_step" <<std::endl;
  #endif
  // htif_t::idle_single_step();
  idle_single_step();
}

