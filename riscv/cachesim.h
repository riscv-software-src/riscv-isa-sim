// See LICENSE for license details.

#ifndef _RISCV_CACHE_SIM_H
#define _RISCV_CACHE_SIM_H

#include "memtracer.h"
#include "eviction_policies.h"
#include <cstring>
#include <string>
#include <map>
#include <cstdint>

class cache_sim_t
{
 public:
  cache_sim_t(const char* config, const char* name);
  cache_sim_t(size_t sets, size_t ways, size_t linesz, const char* name);
  cache_sim_t(size_t sets, size_t ways, size_t linesz, const char* name, const std::string eviction_policy);
  cache_sim_t(const cache_sim_t& rhs);
  virtual ~cache_sim_t();

  void access(uint64_t addr, size_t bytes, bool store);
  void print_stats();
  void set_miss_handler(cache_sim_t* mh) { miss_handler = mh; }
  void set_log(bool _log) { log = _log; }

 protected:
  const uint64_t VALID = 1ULL << 63;
  const uint64_t DIRTY = 1ULL << 62;

  eviction_policy_t* create_eviction_policy(const std::string eviction_policy);
  bool policy_is_valid(const std::string eviction_policy);
  void help();
  int get_way(uint64_t addr);

  virtual uint64_t* check_tag(uint64_t addr);
  virtual uint64_t victimize(uint64_t addr);

  eviction_policy_t* policy;
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
  bool log;

  void init(const std::string eviction_policy);
};

class cache_memtracer_t : public memtracer_t
{
 public:
  cache_memtracer_t(const char* config, const char* name)
  {
    cache = new cache_sim_t(config, name);
  }
  ~cache_memtracer_t()
  {
    delete cache;
  }
  void set_miss_handler(cache_sim_t* mh)
  {
    cache->set_miss_handler(mh);
  }
  void set_log(bool log)
  {
    cache->set_log(log);
  }

 protected:
  cache_sim_t* cache;
};

class icache_sim_t : public cache_memtracer_t
{
 public:
  icache_sim_t(const char* config) : cache_memtracer_t(config, "I$") {}
  bool interested_in_range(uint64_t begin, uint64_t end, access_type type)
  {
    return type == FETCH;
  }
  void trace(uint64_t addr, size_t bytes, access_type type)
  {
    if (type == FETCH) cache->access(addr, bytes, false);
  }
};

class dcache_sim_t : public cache_memtracer_t
{
 public:
  dcache_sim_t(const char* config) : cache_memtracer_t(config, "D$") {}
  bool interested_in_range(uint64_t begin, uint64_t end, access_type type)
  {
    return type == LOAD || type == STORE;
  }
  void trace(uint64_t addr, size_t bytes, access_type type)
  {
    if (type == LOAD || type == STORE) cache->access(addr, bytes, type == STORE);
  }
};

#endif
