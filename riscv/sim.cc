// See LICENSE for license details.

#include "sim.h"
#include "htif.h"
#include <sys/mman.h>
#include <map>
#include <iostream>
#include <climits>
#include <assert.h>
#include <unistd.h>

#ifdef __linux__
# define mmap mmap64
#endif

sim_t::sim_t(int _nprocs, int mem_mb, const std::vector<std::string>& args)
  : htif(new htif_isasim_t(this, args)),
    procs(_nprocs), current_step(0), current_proc(0)
{
  // allocate target machine's memory, shrinking it as necessary
  // until the allocation succeeds
  size_t memsz0 = (size_t)mem_mb << 20;
  if (memsz0 == 0)
    memsz0 = 1L << (sizeof(size_t) == 8 ? 32 : 30);

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

  if (_nprocs == 0)
    _nprocs = 1;
  for (size_t i = 0; i < _nprocs; i++)
    procs[i] = new processor_t(this, new mmu_t(mem, memsz), i);
}

sim_t::~sim_t()
{
  for (size_t i = 0; i < procs.size(); i++)
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
  if (who < procs.size())
    procs[who]->deliver_ipi();
}

reg_t sim_t::get_scr(int which)
{
  switch (which)
  {
    case 0: return num_cores();
    case 1: return memsz >> 20;
    default: return -1;
  }
}

void sim_t::run(bool debug)
{
  while (!htif->done())
  {
    if(!debug)
      step(INTERLEAVE, false);
    else
      interactive();
  }
}

void sim_t::step(size_t n, bool noisy)
{
  for (size_t i = 0, steps = 0; i < n; i += steps)
  {
    htif->tick();

    steps = std::min(n - i, INTERLEAVE - current_step);
    procs[current_proc]->step(steps, noisy);

    current_step += steps;
    if (current_step == INTERLEAVE)
    {
      current_step = 0;
      procs[current_proc]->mmu.yield_load_reservation();
      if (++current_proc == num_cores())
        current_proc = 0;
    }
  }
}
