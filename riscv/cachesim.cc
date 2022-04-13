// See LICENSE for license details.

#include "cachesim.h"
#include "common.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <math.h>

cache_sim_t::cache_sim_t(size_t _sets, size_t _ways, size_t _linesz, const char* _name, const std::string eviction_policy)
: sets(_sets), ways(_ways), linesz(_linesz), name(_name), log(false)
{
  init(eviction_policy);
}

cache_sim_t::cache_sim_t(size_t _sets, size_t _ways, size_t _linesz, const char* _name)
: sets(_sets), ways(_ways), linesz(_linesz), name(_name), log(false)
{
  init(std::string("lfsr"));
}

bool cache_sim_t::policy_is_valid(const std::string eviction_policy)
{
  return (!(eviction_policy.compare("lfsr"))) |
         (!(eviction_policy.compare("lru"))) |
         (!(eviction_policy.compare("fifo"))) |
         (!(eviction_policy.compare("lip"))) |
         (!(eviction_policy.compare("bip")));
}

eviction_policy_t* cache_sim_t::create_eviction_policy(const std::string eviction_policy)
{
  eviction_policy_t* policy = NULL;
  if (!(eviction_policy.compare("lfsr")))
    policy = new lfsr_t(sets, ways);
  else if (!(eviction_policy.compare("lru")))
    policy = new lru_t(sets, ways);
  else if (!(eviction_policy.compare("fifo")))
    policy = new fifo_t(sets, ways);
  else if (!(eviction_policy.compare("lip")))
    policy = new lip_t(sets, ways);
  else if (!(eviction_policy.compare("bip")))
    policy = new bip_t(sets, ways);
  return policy;
}

void cache_sim_t::help()
{
  std::cerr << "Cache configurations must be of the form" << std::endl;
  std::cerr << "  sets:ways:blocksize:policy" << std::endl;
  std::cerr << "where sets, ways, and blocksize are positive integers, with" << std::endl;
  std::cerr << "sets and blocksize both powers of two and blocksize at least 8." << std::endl;
  std::cerr << "Finally, policy is a string. Either 'lfsr', 'lru', 'fifo', 'lip', or 'bip'." << std::endl;
  exit(1);
}

cache_sim_t::cache_sim_t(const char* config, const char* name) : name(name)
{
  const char* wp = strchr(config, ':');
  if (!wp++) help();
  const char* bp = strchr(wp, ':');
  if (!bp++) help();
  const char* eviction_policy = strchr(bp, ':');
  if (!eviction_policy++) help();
  if (!policy_is_valid(std::string(eviction_policy))) help();

  sets = atoi(std::string(config, wp).c_str());
  ways = atoi(std::string(wp, bp).c_str());
  linesz = atoi(std::string(bp, eviction_policy).c_str());

  init(eviction_policy);
}

void cache_sim_t::init(const std::string eviction_policy)
{
  if(sets == 0 || (sets & (sets-1)))
    help();
  if(linesz < 8 || (linesz & (linesz-1)))
    help();

  idx_shift = std::log2(linesz);

  tags = new uint64_t[sets*ways]();
  read_accesses = 0;
  read_misses = 0;
  bytes_read = 0;
  write_accesses = 0;
  write_misses = 0;
  bytes_written = 0;
  writebacks = 0;

  miss_handler = NULL;
  policy = create_eviction_policy(eviction_policy);
}

cache_sim_t::cache_sim_t(const cache_sim_t& rhs)
 : sets(rhs.sets), ways(rhs.ways), linesz(rhs.linesz),
   idx_shift(rhs.idx_shift), name(rhs.name), log(false)
{
  tags = new uint64_t[sets*ways];
  memcpy(tags, rhs.tags, sets*ways*sizeof(uint64_t));
}

cache_sim_t::~cache_sim_t()
{
  print_stats();
  delete [] tags;
  delete policy;
}

void cache_sim_t::print_stats()
{
  if(read_accesses + write_accesses == 0)
    return;

  float mr = 100.0f*(read_misses+write_misses)/(read_accesses+write_accesses);

  std::cout << std::setprecision(3) << std::fixed;
  std::cout << name << "\t";
  std::cout << "Bytes Read:            " << bytes_read << std::endl;
  std::cout << name << "\t";
  std::cout << "Bytes Written:         " << bytes_written << std::endl;
  std::cout << name << "\t";
  std::cout << "Read Accesses:         " << read_accesses << std::endl;
  std::cout << name << "\t";
  std::cout << "Write Accesses:        " << write_accesses << std::endl;
  std::cout << name << "\t";
  std::cout << "Read Misses:           " << read_misses << std::endl;
  std::cout << name << "\t";
  std::cout << "Write Misses:          " << write_misses << std::endl;
  std::cout << name << "\t";
  std::cout << "Writebacks:            " << writebacks << std::endl;
  std::cout << name << "\t";
  std::cout << "Miss Rate:             " << mr << '%' << std::endl;
}

uint64_t* cache_sim_t::check_tag(uint64_t addr)
{
  size_t idx = (addr >> idx_shift) & (sets-1);
  size_t tag = (addr >> idx_shift) | VALID;

  for (size_t i = 0; i < ways; i++)
    if (tag == (tags[idx*ways + i] & ~DIRTY))
      return &tags[idx*ways + i];

  return NULL;
}

// Returns tag of victimized cacheline AND write new cacheline tag instead of
// the existing one!
uint64_t cache_sim_t::victimize(uint64_t addr)
{
  // Find set via 'idx'
  size_t idx = (addr >> idx_shift) & (sets-1);
  // Get index of way to evict
  size_t way = policy->next(idx);
  // Store cache-line's tag to be evicted
  uint64_t victim = tags[idx*ways + way];
  // Replace evicted cache-line's tag with new one
  tags[idx*ways + way] = (addr >> idx_shift) | VALID;
  // Tell the eviction policy which metadata to change
  policy->insert(idx, way);
  return victim;
}

int cache_sim_t::get_way(uint64_t addr)
{
  size_t idx = (addr >> idx_shift) & (sets-1);
  size_t tag = (addr >> idx_shift) | VALID;
  int way = -1;
  for (size_t i = 0; i < ways; i++)
    if (tag == (tags[idx*ways+i] & ~DIRTY))
      way = i;
  return way;
}

void cache_sim_t::access(uint64_t addr, size_t bytes, bool store)
{
  store ? write_accesses++ : read_accesses++;
  (store ? bytes_written : bytes_read) += bytes;

  uint64_t* hit_way = check_tag(addr);
  if (likely(hit_way != NULL))
  {
    if (store)
      *hit_way |= DIRTY;
    int way = get_way(addr);
    // If cache-hit (note that a hit is expected because of previous condition).
    if (way != -1)
        policy->update(addr, way, idx_shift);
    return;
  }

  store ? write_misses++ : read_misses++;
  if (log)
  {
    std::cerr << name << " "
              << (store ? "write" : "read") << " miss 0x"
              << std::hex << addr << std::endl;
  }

  // Victimize AND insert at 'addr'
  uint64_t victim = victimize(addr);

  if ((victim & (VALID | DIRTY)) == (VALID | DIRTY))
  {
    uint64_t dirty_addr = (victim & ~(VALID | DIRTY)) << idx_shift;
    if (miss_handler)
      miss_handler->access(dirty_addr, linesz, true);
    writebacks++;
  }

  if (miss_handler)
    miss_handler->access(addr & ~(linesz-1), linesz, false);

  if (store)
    *check_tag(addr) |= DIRTY;
}
