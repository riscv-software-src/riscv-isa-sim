#ifndef _RISCV_ICSIM_H
#define _RISCV_ICSIM_H

#include <cstring>
#include <stdint.h>

class icsim_t
{
public:
  icsim_t(size_t sets, size_t ways, size_t linesz);
  ~icsim_t();

  void tick(uint64_t pc, int insnlen);
private:
  size_t sets;
  size_t ways;
  size_t linesz;
  size_t idx_shift;
  size_t idx_mask;
  
  uint64_t accesses;
  uint64_t misses;
  uint64_t bytes_fetched;

  uint64_t* tags;

  static const uint64_t VALID = 1ULL << 63;
};

#endif
