// See LICENSE for license details.

#ifndef _RISCV_EVICTION_POLICIES_H
#define _RISCV_EVICTION_POLICIES_H

#include <stdlib.h>
#include <cstdint>

class eviction_policy_t
{
protected:
  size_t sets;
  size_t ways;
public:
  eviction_policy_t(size_t sets, size_t ways) : sets(sets), ways(ways) {}
  eviction_policy_t(const eviction_policy_t& policy) : sets(policy.sets), ways(policy.ways) {}
  virtual uint32_t next(size_t idx) = 0;
  virtual void update(uint64_t addr, uint32_t way, const size_t idx_shift) = 0;
  virtual void insert(size_t idx, size_t way) = 0;
  virtual ~eviction_policy_t() {};
};

class lfsr_t : public eviction_policy_t
{
private:
  uint32_t reg;
public:
  lfsr_t(size_t sets, size_t ways) : eviction_policy_t(sets, ways), reg(0xACE1u) {}
  lfsr_t(const lfsr_t& lfsr) : eviction_policy_t(lfsr), reg(lfsr.reg) {}
  uint32_t next(size_t idx) override;
  void update(uint64_t addr, uint32_t way, const size_t idx_shift) override;
  void insert(size_t idx, size_t way) override;
  ~lfsr_t() {};
};

class lru_t : public eviction_policy_t
{
protected:
  uint32_t* metadata;
  uint32_t lowest_recency_way(size_t idx);
  void promote(size_t idx, size_t way, uint32_t recency);
  void set_as_mru(size_t idx, size_t way);
public:
  lru_t(size_t sets, size_t ways) : eviction_policy_t(sets, ways) {
    metadata = new uint32_t[sets*ways]();
  }
  uint32_t next(size_t idx) override;
  void update(uint64_t addr, uint32_t way, const size_t idx_shift) override;
  void insert(size_t idx, size_t way) override;
  ~lru_t() {
    delete [] metadata;
  };
};

class fifo_t : public lru_t
{
public:
  fifo_t(size_t sets, size_t ways) : lru_t(sets, ways) {}
  void update(uint64_t addr, uint32_t way, const size_t idx_shift) override;
  void insert(size_t idx, size_t way) override;
  ~fifo_t() {};
};

class lip_t : public lru_t
{
public:
  lip_t(size_t sets, size_t ways) : lru_t(sets, ways) {}
  void update(uint64_t addr, uint32_t way, const size_t idx_shift) override;
  void insert(size_t idx, size_t way) override;
  ~lip_t() {};
};

class bip_t : public lip_t
{
private:
  uint32_t threshold;
  eviction_policy_t* lfsr;
public:
  bip_t(size_t sets, size_t ways, uint32_t threshold) : lip_t(sets, ways), threshold(threshold)
  {
    lfsr = new lfsr_t(sets, ways);
  }
  bip_t(size_t sets, size_t ways) : bip_t(sets, ways, 8) {}
  void insert(size_t idx, size_t way) override;
  ~bip_t()
  {
    delete lfsr;
  };
};

#endif
