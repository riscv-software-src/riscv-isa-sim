#include "snapshot.h"
#include <fstream>

using namespace std;
std::vector<std::pair<reg_t, mem_t *>> snapshot_t::mem_restore() {
  return mems;
}

state_t *snapshot_t::get_state(int i) { return cpu_states[i]; }

int snapshot_t::get_procs() { return procs; }

snapshot_t::snapshot_t(const char *path) 
{
  int fd = open(path, O_RDONLY);
  struct stat s;
  assert(fd != -1);
  if (fstat(fd, &s) < 0)
    abort();
  size_t size = s.st_size;
  char *buf = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(buf != MAP_FAILED);
  close(fd);
  assert(size >= sizeof(Snapshot_Head));
  const Snapshot_Head *sh = (const Snapshot_Head *)buf;
  procs = sh->procs;
  mems = getmem((const char *)(buf + sh->memoff),
                (const char *)(buf + sh->memend));
  uint64_t offset = sh->cpuoff;
  std::vector<state_t *> res;
  for (int i = 0; i < procs; i++) {
    char *tmp = (char *)calloc(1, sizeof(state_t));
    memcpy(tmp, buf + offset, sizeof(state_t));
    offset += sizeof(state_t);
    res.push_back((state_t *)tmp);
  }
  cpu_states = res;

  memcpy(&mtime, buf + sh->cloff, sizeof(mtime_t));
  mtimecmp_t tmp;
  for (int i = 0; i < procs; i++) {
    memcpy(&tmp, buf + sh->cloff + sizeof(mtime_t) + i * sizeof(mtimecmp_t),
           sizeof(mtimecmp_t));
    mtimecmp.push_back(tmp);
  }
  close(fd);
}

snapshot_t::snapshot_t(sim_t *sim, mmu_t *mmu) 
{
  mems = sim->get_mems();
  procs = sim->nprocs();
  for (int i = 0; i < procs; i++) {
    cpu_states.push_back(sim->get_core(i)->get_state());
    mtimecmp.push_back(mmu->load_uint64(
        sim->get_clint() + MTIMECMP_BASE + (uint64_t)sizeof(mtimecmp_t) * i));
  }
  mtime =
      mmu->load_uint64(sim->get_clint() + MTIME_BASE);
}

void snapshot_t::save(const char *path) 
{
  Snapshot_Head sh;
  ofstream out;
  out.open(path, ios::binary);
  out.write((char *)&sh, sizeof(Snapshot_Head));
  sh.memoff = out.tellp();
  ramdump(out);
  sh.memend = out.tellp();
  sh.cpuoff = out.tellp();
  sh.procs = procs;
  for (int i = 0; i < procs; i++)
    out.write((char *)cpu_states[i], sizeof(state_t));
  sh.cloff = out.tellp();
  // out.write((char *)&clint,sizeof(clint_t));
  out.write((char *)&mtime, sizeof(mtime_t));
  for (int i = 0; i < procs; i++) {
    out.write((char *)&mtimecmp[i], sizeof(mtimecmp_t));
  }
  out.seekp(0, ios::beg);
  out.write((char *)&sh, sizeof(Snapshot_Head));
  out.close();
}