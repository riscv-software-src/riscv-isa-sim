// See LICENSE for license details.

#include "cachesim.h"
#include "common.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>


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

  tags.resize(sets);
  for (size_t i = 0; i < tags.size(); i++)
    tags[i].resize(ways);

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
   tags(rhs.tags), name(rhs.name), log(false)
{}

cache_sim_t::~cache_sim_t()
{
  print_stats();
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

cache_sim_addr_t* cache_sim_t::check_tag(cache_sim_addr_t& addr)
{
  for (size_t i = 0; i < ways; i++)
    if (addr.tag == tags[addr.idx][i].tag)
      return &tags[addr.idx][i];
  return NULL;
}

// Returns tag of victimized cacheline AND write new cacheline tag instead of
// the existing one!
cache_sim_addr_t cache_sim_t::victimize(cache_sim_addr_t& addr)
{
  // Get index of way to evict
  size_t way = policy->next(addr.idx);
  // Store cache-line's tag to be evicted
  cache_sim_addr_t victim = tags[addr.idx][way];
  // Replace evicted cache-line's tag with new one
  tags[addr.idx][way] = addr;
  tags[addr.idx][way].set_valid();
  // Tell the eviction policy which metadata to change
  policy->insert(addr.idx, way);
  return victim;
}

uint32_t cache_sim_t::get_way(cache_sim_addr_t& addr)
{
  auto begin = tags[addr.idx].begin();
  auto end = tags[addr.idx].end();
  return std::distance(begin, std::find(begin, end, addr));
}

void cache_sim_t::access(uint64_t raw_addr, size_t bytes, bool store)
{
  store ? write_accesses++ : read_accesses++;
  (store ? bytes_written : bytes_read) += bytes;

  cache_sim_addr_t addr = cache_sim_addr_t(raw_addr, this->sets, this->linesz);

  cache_sim_addr_t* hit_way = check_tag(addr);
  if (likely(hit_way != NULL))
  {
    if (store)
      hit_way->set_dirty();
    policy->update(addr, get_way(addr));
    return;
  }

  store ? write_misses++ : read_misses++;
  if (log)
  {
    std::cerr << name << " "
              << (store ? "write" : "read") << " miss 0x"
              << std::hex << addr.to_uint64(this->sets, this->linesz)
              << std::endl;
  }

  // Victimize AND insert at 'addr'
  cache_sim_addr_t victim = victimize(addr);

  if (victim.is_valid() & victim.is_dirty())
  {
    uint64_t dirty_addr = victim.to_uint64(this->sets, this->linesz);
    if (miss_handler)
      miss_handler->access(dirty_addr, linesz, true);
    writebacks++;
  }

  if (miss_handler)
    miss_handler->access(addr.to_uint64(this->sets, this->linesz), linesz, false);

  if (store)
    check_tag(addr)->set_dirty();
}
