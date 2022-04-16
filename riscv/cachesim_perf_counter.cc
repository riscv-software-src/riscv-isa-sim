#include "cachesim_perf_counter.h"
#include <iostream>
#include <iomanip>

cachesim_perf_counter::cachesim_perf_counter()
{
  name = "";
  read_accesses  = 0;
  read_misses    = 0;
  bytes_read     = 0;
  write_accesses = 0;
  write_misses   = 0;
  bytes_written  = 0;
  writebacks     = 0;
}

cachesim_perf_counter::cachesim_perf_counter(const cachesim_perf_counter& perf):
  name(perf.name), read_accesses(perf.read_accesses),
  read_misses(perf.read_misses), bytes_read(perf.bytes_read),
  write_accesses(perf.write_accesses), write_misses(perf.write_misses),
  bytes_written(perf.bytes_written) {};

void cachesim_perf_counter::set_name(const std::string& name_str)
{
    name = name_str;
}

void cachesim_perf_counter::access(const bool& store, const uint32_t& bytes)
{
  if(store)
  {
    write_accesses++;
    bytes_written += bytes;
  }
  else
  {
    read_accesses++;
    bytes_read += bytes;
  }
}

void cachesim_perf_counter::miss(const bool& store)
{
  if(store)
    write_misses++;
  else
    read_misses++;
}

void cachesim_perf_counter::writeback()
{
  writebacks++;
}

cachesim_perf_counter::~cachesim_perf_counter()
{
  if((read_accesses+write_accesses) > 0)
  {
    float mr = 100.0f*(read_misses+write_misses)/(read_accesses+write_accesses);
    std::cout << std::setprecision(3)          << std::fixed;
    std::cout << name << "\tBytes Read:\t"     << bytes_read     << std::endl;
    std::cout << name << "\tBytes Written:\t"  << bytes_written  << std::endl;
    std::cout << name << "\tRead Accesses:\t"  << read_accesses  << std::endl;
    std::cout << name << "\tWrite Accesses:\t" << write_accesses << std::endl;
    std::cout << name << "\tRead Misses:\t"    << read_misses    << std::endl;
    std::cout << name << "\tWrite Misses:\t"   << write_misses   << std::endl;
    std::cout << name << "\tWritebacks:\t"     << writebacks     << std::endl;
    std::cout << name << "\tMiss Rate:\t"      << mr << '%'      << std::endl;
  }
}
