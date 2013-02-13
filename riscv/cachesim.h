#ifndef _RISCV_CACHE_SIM_H
#define _RISCV_CACHE_SIM_H

#include "memtracer.h"
#include <cstring>
#include <string>
#include <stdint.h>

class lfsr_t
{
 public:
  lfsr_t() : reg(1) {}
  lfsr_t(const lfsr_t& lfsr) : reg(lfsr.reg) {}
  uint32_t next() { return reg = (reg>>1)^(-(reg&1) & 0xd0000001); }
 private:
  uint32_t reg;
};

class cache_sim_t
{
 public:
  cache_sim_t(const char* config, const char* name);
  cache_sim_t(size_t sets, size_t ways, size_t linesz, const char* name);
  cache_sim_t(const cache_sim_t& rhs);
  ~cache_sim_t();

  void access(uint64_t addr, size_t bytes, bool store);
  void print_stats();
  void set_miss_handler(cache_sim_t* mh) { miss_handler = mh; }

 private:
  lfsr_t lfsr;
  cache_sim_t* miss_handler;

  size_t sets;
  size_t ways;
  size_t linesz;
  size_t idx_shift;

  uint64_t* tags;
  
  uint64_t read_accesses;
  uint64_t read_misses;
  uint64_t bytes_read;
  uint64_t write_accesses;
  uint64_t write_misses;
  uint64_t bytes_written;
  uint64_t writebacks;

  std::string name;

  static const uint64_t VALID = 1ULL << 63;
  static const uint64_t DIRTY = 1ULL << 62;

  void init();
};

class unified_cache_sim_t : public memtracer_t
{
 public:
  unified_cache_sim_t(const char* config) : cache(config, "U$") {}
  bool interested_in_range(size_t begin, size_t end, bool store, bool fetch)
  {
    return true;
  }
  void trace(uint64_t addr, size_t bytes, bool store, bool fetch)
  {
    cache.access(addr, bytes, store);
  }
 private:
  cache_sim_t cache;
};

class icache_sim_t : public memtracer_t, public cache_sim_t
{
 public:
  icache_sim_t(const char* config) : cache_sim_t(config, "I$") {}
  bool interested_in_range(size_t begin, size_t end, bool store, bool fetch)
  {
    return fetch;
  }
  void trace(uint64_t addr, size_t bytes, bool store, bool fetch)
  {
    if (fetch) access(addr, bytes, false);
  }
};

class dcache_sim_t : public memtracer_t, public cache_sim_t
{
 public:
  dcache_sim_t(const char* config) : cache_sim_t(config, "D$") {}
  bool interested_in_range(size_t begin, size_t end, bool store, bool fetch)
  {
    return !fetch;
  }
  void trace(uint64_t addr, size_t bytes, bool store, bool fetch)
  {
    if (!fetch) access(addr, bytes, store);
  }
};

#endif
