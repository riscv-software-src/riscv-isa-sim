#include "sim.h"
#include "htif.h"
#include <sys/mman.h>
#include <map>
#include <iostream>
#include <climits>
#include <assert.h>

#ifdef __linux__
# define mmap mmap64
#endif

sim_t::sim_t(int _nprocs, htif_t* _htif)
  : htif(_htif),
    tohost(0),
    fromhost(0),
    procs(_nprocs),
    running(false)
{
  // allocate target machine's memory, shrinking it as necessary
  // until the allocation succeeds

  size_t memsz0 = sizeof(size_t) == 8 ? 0x100000000ULL : 0x70000000UL;
  size_t quantum = std::max(PGSIZE, (reg_t)sysconf(_SC_PAGESIZE));
  memsz0 = memsz0/quantum*quantum;

  memsz = memsz0;
  mem = (char*)mmap(NULL, memsz, PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);

  if(mem == MAP_FAILED)
  {
    while(mem == MAP_FAILED && (memsz = memsz*10/11/quantum*quantum))
      mem = (char*)mmap(NULL, memsz, PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    assert(mem != MAP_FAILED);
    fprintf(stderr, "warning: only got %lu bytes of target mem (wanted %lu)\n",
            (unsigned long)memsz, (unsigned long)memsz0);
  }

  mmu = new mmu_t(mem, memsz);

  for(size_t i = 0; i < num_cores(); i++)
    procs[i] = new processor_t(this, new mmu_t(mem, memsz), i);

  htif->init(this);
}

sim_t::~sim_t()
{
  for(size_t i = 0; i < num_cores(); i++)
  {
    mmu_t* pmmu = &procs[i]->mmu;
    delete procs[i];
    delete pmmu;
  }
  delete mmu;
  munmap(mem, memsz);
}

void sim_t::send_ipi(reg_t who)
{
  if(who < num_cores())
    procs[who]->deliver_ipi();
}

void sim_t::run(bool debug)
{
  // word 0 of memory contains the memory capacity in MB
  mmu->store_uint32(0, memsz >> 20);
  // word 1 of memory contains the core count
  mmu->store_uint32(4, num_cores());

  //htif->wait_for_start();

  for(running = true; running; )
  {
    if(!debug)
      step_all(10000, 100, false);
    else
      interactive();
  }
}

void sim_t::step_all(size_t n, size_t interleave, bool noisy)
{
  htif->wait_for_packet();
  for(size_t j = 0; j < n; j+=interleave)
    for(int i = 0; i < (int)num_cores(); i++)
      procs[i]->step(interleave,noisy);
}
