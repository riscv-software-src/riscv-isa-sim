#include "icsim.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>

icsim_t::icsim_t(size_t _sets, size_t _ways, size_t _linesz, const char* _name)
 : sets(_sets), ways(_ways), linesz(_linesz), idx_mask(_sets-1), name(_name)
{
  if(sets == 0 || (sets & (sets-1)))
    throw std::logic_error("sets not a power of 2");
  if(linesz == 0 || (linesz & (linesz-1)))
    throw std::logic_error("linesz not a power of 2");

  idx_shift = 0;
  while(_linesz >>= 1)
    idx_shift++;

  tags = new uint64_t[sets*ways];
  memset(tags, 0, sets*ways*sizeof(uint64_t));

  read_accesses = 0;
  read_misses = 0;
  bytes_read = 0;
  write_accesses = 0;
  write_misses = 0;
  bytes_written = 0;
  writebacks = 0;
}

icsim_t::icsim_t(const icsim_t& rhs)
 : sets(rhs.sets), ways(rhs.ways), linesz(rhs.linesz),
   idx_shift(rhs.idx_shift), idx_mask(rhs.idx_mask), name(rhs.name)
{
  tags = new uint64_t[sets*ways];
  memcpy(tags, rhs.tags, sets*ways*sizeof(uint64_t));
}

icsim_t::~icsim_t()
{
  delete [] tags;
}

void icsim_t::print_stats()
{
  if(read_accesses + write_accesses == 0)
    return;

  float mr = 100.0f*(read_misses+write_misses)/(read_accesses+write_accesses);

  std::cout << std::setprecision(3) << std::fixed;
  std::cout << name << " ";
  std::cout << "Bytes Read:            " << bytes_read << std::endl;
  std::cout << name << " ";
  std::cout << "Bytes Written:         " << bytes_written << std::endl;
  std::cout << name << " ";
  std::cout << "Read Accesses:         " << read_accesses << std::endl;
  std::cout << name << " ";
  std::cout << "Write Accesses:        " << write_accesses << std::endl;
  std::cout << name << " ";
  std::cout << "Read Misses:           " << read_misses << std::endl;
  std::cout << name << " ";
  std::cout << "Write Misses:          " << write_misses << std::endl;
  std::cout << name << " ";
  std::cout << "Writebacks:            " << writebacks << std::endl;
  std::cout << name << " ";
  std::cout << "Miss Rate:             " << mr << '%' << std::endl;

  float cr = read_accesses == 0 ? 0.0f : 100.0f*bytes_read/(4*read_accesses);
  if(name == "I$")
  {
    std::cout << name << " ";
    std::cout << "RVC compression ratio: " << cr << '%' << std::endl;
  }
}

void icsim_t::tick(uint64_t pc, int insnlen, bool store)
{
  store ? write_accesses++ : read_accesses++;
  (store ? bytes_written : bytes_read) += insnlen;

  size_t idx = (pc >> idx_shift) & idx_mask;
  size_t tag = (pc >> idx_shift) | VALID;

  for(size_t i = 0; i < ways; i++)
  {
    if(tag == (tags[idx + i*sets] & ~DIRTY)) // hit
    {
      if(store)
        tags[idx + i*sets] |= DIRTY;
      return;
    }
  }

  store ? write_misses++ : read_misses++;

  size_t way = lfsr.next() % ways;
  if((tags[idx + way*sets] & (VALID | DIRTY)) == (VALID | DIRTY))
    writebacks++;
  tags[idx + way*sets] = tag;
}
