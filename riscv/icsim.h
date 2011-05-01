#ifndef _RISCV_ICSIM_H
#define _RISCV_ICSIM_H

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

class icsim_t
{
public:
  icsim_t(size_t sets, size_t ways, size_t linesz, const char* name);
  icsim_t(const icsim_t& rhs);
  ~icsim_t();

  void tick(uint64_t pc, int insnlen, bool store);
  void print_stats();
private:
  lfsr_t lfsr;

  size_t sets;
  size_t ways;
  size_t linesz;
  size_t idx_shift;
  size_t idx_mask;

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
};

#endif
