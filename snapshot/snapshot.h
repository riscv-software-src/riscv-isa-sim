#ifndef SNAP_H
#define SNAP_H
#include "device.h"
#include "mmu.h"
#include "sim.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>

typedef struct {
  uint64_t memoff;
  uint64_t memend;
  uint64_t cpuoff;
  uint64_t cloff;
  int procs;
} Snapshot_Head;

std::vector<std::pair<reg_t, char *>> getmem(const char *start,
                                              const char *end);
class snapshot_t 
{
public:
  snapshot_t(const char *path);
  snapshot_t(sim_t *sim, mmu_t *mmu);
  snapshot_t() {}
  void save(const char *path);
  bool ramdump(std::ofstream &out);
  state_t *get_state(int i);
  int get_procs();
  std::vector<mtimecmp_t> mtimecmp;
  mtime_t mtime;
  std::map<reg_t, bool> *tags;
  mmu_t * mmu;
  sim_t * sim;
  std::vector<std::pair<reg_t, char *>> mems;
private:
  std::vector<state_t *> cpu_states;
  int procs;
};

#endif
