#ifndef _RISCV_CACHE_SIM_ADDR_H
#define _RISCV_CACHE_SIM_ADDR_H

#include <cstdint>

class cache_sim_addr_t
{
public:
  bool valid;
  bool dirty;
  uint64_t tag;
  uint64_t idx;

  cache_sim_addr_t();
  cache_sim_addr_t(const uint64_t& addr, const uint32_t& sets, const uint32_t& linesz);
  cache_sim_addr_t(const cache_sim_addr_t& addr);
  bool operator==(const cache_sim_addr_t& other);
  bool operator<(const cache_sim_addr_t& other);
  void next_cacheline(const uint32_t& sets);
  void set_valid();
  bool is_valid();
  void set_invalid();
  void set_dirty();
  bool is_dirty();
  void set_clean();
  uint64_t to_uint64(const uint32_t& sets, const uint32_t& linesz);
  virtual ~cache_sim_addr_t();
};

#endif
