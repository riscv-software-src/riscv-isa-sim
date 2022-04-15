// See LICENSE for license details.

#ifndef _RISCV_EVICTION_POLICIES_H
#define _RISCV_EVICTION_POLICIES_H

#include <vector>
#include <stdlib.h>
#include <cstdint>
#include "cachesim_addr.h"

class eviction_policy_t
{
protected:
  size_t sets;
  size_t ways;
public:
  eviction_policy_t(const size_t& sets, const size_t& ways) : sets(sets), ways(ways) {}
  eviction_policy_t(const eviction_policy_t& policy) : sets(policy.sets), ways(policy.ways) {}
  virtual uint32_t next(const size_t& idx) = 0;
  virtual void update(const cache_sim_addr_t& addr, const uint32_t& way) = 0;
  virtual void insert(const size_t& idx, const size_t& way) = 0;
  virtual ~eviction_policy_t() {};
};

class lfsr_t : public eviction_policy_t
{
private:
  uint32_t reg;
public:
  lfsr_t(const size_t& sets, const size_t& ways) : eviction_policy_t(sets, ways), reg(0xACE1u) {}
  lfsr_t(const lfsr_t& lfsr) : eviction_policy_t(lfsr), reg(lfsr.reg) {}
  uint32_t next(const size_t& idx) override;
  void update(const cache_sim_addr_t& addr, const uint32_t& way) override;
  void insert(const size_t& idx, const size_t& way) override;
  ~lfsr_t() {};
};

class lru_t : public eviction_policy_t
{
protected:
  std::vector<std::vector<uint32_t>> metadata;
  uint32_t lowest_recency_way(const size_t& idx);
  void promote(const size_t& idx, const size_t& way, const uint32_t& recency);
  void set_as_mru(const size_t& idx, const size_t& way);
public:
  lru_t(const size_t& sets, const size_t& ways) : eviction_policy_t(sets, ways) {
    metadata.resize(sets);
    for (size_t i = 0; i < metadata.size(); i++)
        metadata[i].resize(ways);
  }
  uint32_t next(const size_t& idx) override;
  void update(const cache_sim_addr_t& addr, const uint32_t& way) override;
  void insert(const size_t& idx, const size_t& way) override;
  ~lru_t() {};
};

class fifo_t : public lru_t
{
public:
  fifo_t(const size_t& sets, const size_t& ways) : lru_t(sets, ways) {}
  void update(const cache_sim_addr_t& addr, const uint32_t& way) override;
  ~fifo_t() {};
};

class lip_t : public lru_t
{
public:
  lip_t(const size_t& sets, const size_t& ways) : lru_t(sets, ways) {}
  void update(const cache_sim_addr_t& addr, const uint32_t& way) override;
  void insert(const size_t& idx, const size_t& way) override;
  ~lip_t() {};
};

class bip_t : public lip_t
{
private:
  uint32_t threshold;
  eviction_policy_t* lfsr;
public:
  bip_t(const size_t& sets, const size_t& ways, const uint32_t& threshold) : lip_t(sets, ways), threshold(threshold)
  {
    lfsr = new lfsr_t(sets, ways);
  }
  bip_t(const size_t& sets, const size_t& ways) : bip_t(sets, ways, 8) {}
  void insert(const size_t& idx, const size_t& way) override;
  ~bip_t()
  {
    delete lfsr;
  };
};

#endif
