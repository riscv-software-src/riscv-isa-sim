#ifndef _RISCV_CACHE_SIM_PERF_COUNTER_H
#define _RISCV_CACHE_SIM_PERF_COUNTER_H

#include <cstdint>
#include <string>

class cachesim_perf_counter
{
private:
  std::string name;
  uint64_t read_accesses;
  uint64_t read_misses;
  uint64_t bytes_read;
  uint64_t write_accesses;
  uint64_t write_misses;
  uint64_t bytes_written;
  uint64_t writebacks;
public:
  cachesim_perf_counter();
  cachesim_perf_counter(const std::string& name_str);
  cachesim_perf_counter(const cachesim_perf_counter& perf);
  void set_name(const std::string& name_str);
  void access(const bool& store, const uint32_t& bytes);
  void miss(const bool& store);
  void writeback();
  virtual ~cachesim_perf_counter();
};

#endif
