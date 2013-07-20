// See LICENSE for license details.

#include "sim.h"
#include "htif.h"
#include <map>
#include <iostream>
#include <climits>
#include <cstdlib>
#include <cassert>

sim_t::sim_t(size_t _nprocs, size_t mem_mb, const std::vector<std::string>& args)
  : htif(new htif_isasim_t(this, args)),
    procs(_nprocs), current_step(0), current_proc(0), debug(false)
{
  // allocate target machine's memory, shrinking it as necessary
  // until the allocation succeeds
  size_t memsz0 = (size_t)mem_mb << 20;
  size_t quantum = 1L << 20;
  if (memsz0 == 0)
    memsz0 = 1L << (sizeof(size_t) == 8 ? 32 : 30);

  memsz = memsz0;
  while ((mem = (char*)calloc(1, memsz)) == NULL)
    memsz = memsz*10/11/quantum*quantum;

  if (memsz != memsz)
    fprintf(stderr, "warning: only got %lu bytes of target mem (wanted %lu)\n",
            (unsigned long)memsz, (unsigned long)memsz0);

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
  free(mem);
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
    case 0: return procs.size();
    case 1: return memsz >> 20;
    default: return -1;
  }
}

void sim_t::run()
{
  while (!htif->done())
  {
    if (debug)
      interactive();
    else
      step(INTERLEAVE, false);
  }
}

void sim_t::step(size_t n, bool noisy)
{
  for (size_t i = 0, steps = 0; i < n; i += steps)
  {
    htif->tick();
    if (!running())
      break;

    steps = std::min(n - i, INTERLEAVE - current_step);
    procs[current_proc]->step(steps, noisy);

    current_step += steps;
    if (current_step == INTERLEAVE)
    {
      current_step = 0;
      procs[current_proc]->mmu.yield_load_reservation();
      if (++current_proc == procs.size())
        current_proc = 0;
    }
  }
}

bool sim_t::running()
{
  for (size_t i = 0; i < procs.size(); i++)
    if (procs[i]->running())
      return true;
  return false;
}

void sim_t::stop()
{
  procs[0]->tohost = 1;
  while (!htif->done())
    htif->tick();
}
