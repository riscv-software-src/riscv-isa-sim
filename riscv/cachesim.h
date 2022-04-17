// See LICENSE for license details.

#ifndef _RISCV_CACHE_SIM_H
#define _RISCV_CACHE_SIM_H

#include "memtracer.h"
#include "eviction_policies.h"
#include "cachesim_addr.h"
#include "cachesim_perf_counter.h"
#include <cstring>
#include <string>
#include <map>
#include <cstdint>
#include <math.h>
#include <vector>

class cache_sim_t
{
public:
  cache_sim_t(const char* config, const char* name);
  cache_sim_t(size_t sets, size_t ways, size_t linesz, const char* name);
  cache_sim_t(size_t sets, size_t ways, size_t linesz, const char* name, const std::string eviction_policy);
  cache_sim_t(const cache_sim_t& rhs);
  virtual ~cache_sim_t();

  void access(const uint64_t addr, const size_t bytes, const bool store);
  void clean_invalidate(const uint64_t addr, const size_t bytes, const bool clean, const bool inval);
  void print_stats();
  void set_miss_handler(cache_sim_t* mh) { miss_handler = mh; }
  void set_log(bool _log) { log = _log; }

 protected:
  eviction_policy_t* create_eviction_policy(const std::string eviction_policy);
  bool policy_is_valid(const std::string eviction_policy);
  void help();

  int check_tag(cache_sim_addr_t& addr);
  cache_sim_addr_t victimize(const cache_sim_addr_t& addr);

  eviction_policy_t* policy;
  cache_sim_t* miss_handler;

  size_t sets;
  size_t ways;
  size_t linesz;

  std::vector<std::vector<cache_sim_addr_t>> tags;

  cachesim_perf_counter perf_counter;// = cachesim_perf_counter();

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
  void clean_invalidate(uint64_t addr, size_t bytes, bool clean, bool inval)
  {
    cache->clean_invalidate(addr, bytes, clean, inval);
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
