#include "icsim.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>

icsim_t::icsim_t(size_t _sets, size_t _ways, size_t _linesz)
{
  sets = _sets;
  ways = _ways;
  linesz = _linesz;

  if(sets == 0 || (sets & (sets-1)))
    throw std::logic_error("sets not a power of 2");
  if(linesz == 0 || (linesz & (linesz-1)))
    throw std::logic_error("linesz not a power of 2");
  if(ways != 1)
    throw std::logic_error("set associativity currently unsupported");

  idx_mask = sets-1;
  idx_shift = 0;
  while(_linesz >>= 1)
    idx_shift++;

  tags = new uint64_t[sets*ways];
  memset(tags, 0, sets*ways*sizeof(uint64_t));
}

icsim_t::~icsim_t()
{
  float mr = 100.0f*misses/accesses;
  float cr = 100.0f*bytes_fetched/(4*accesses);

  std::cout << "Instruction cache statsistics" << std::endl;
  std::cout << "Bytes fetched: " << bytes_fetched << std::endl;
  std::cout << "Hits: " << (accesses-misses) << std::endl;
  std::cout << "Misses: " << misses << std::endl;
  std::cout << "Miss rate: " << std::setprecision(3) << mr << '%' << std::endl;
  std::cout << "RVC compression ratio: " << cr << '%' << std::endl;

  delete [] tags;
}

void icsim_t::tick(uint64_t pc, int insnlen)
{
  accesses++;
  bytes_fetched += insnlen;

  size_t idx = (pc >> idx_shift) & idx_mask;
  size_t tag = (pc >> idx_shift) | VALID;
  if(tag != tags[idx])
  {
    misses++;
    tags[idx] = tag;
  }
}
