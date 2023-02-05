#include <sys/time.h>
#include "devices.h"
#include "processor.h"

clint_t::clint_t(std::vector<processor_t*>& procs, uint64_t freq_hz, bool real_time)
  : procs(procs), freq_hz(freq_hz), real_time(real_time), mtime(0), mtimecmp(procs.size())
{
  struct timeval base;

  gettimeofday(&base, NULL);

  real_time_ref_secs = base.tv_sec;
  real_time_ref_usecs = base.tv_usec;
}

/* 0000 msip hart 0
 * 0004 msip hart 1
 * 4000 mtimecmp hart 0 lo
 * 4004 mtimecmp hart 0 hi
 * 4008 mtimecmp hart 1 lo
 * 400c mtimecmp hart 1 hi
 * bff8 mtime lo
 * bffc mtime hi
 */

#define MSIP_BASE	0x0
#define MTIMECMP_BASE	0x4000
#define MTIME_BASE	0xbff8

template<typename T>
void partial_write(T* word, reg_t addr, size_t len, const uint8_t* bytes)
{
  for (size_t i = 0; i < len; i++) {
    const int shift = 8 * ((addr + i) % sizeof(T));
    *word = (*word & ~(T(0xFF) << shift)) | (T(bytes[i]) << shift);
  }
}

template<typename T>
void partial_read(T word, reg_t addr, size_t len, uint8_t* bytes)
{
  for (size_t i = 0; i < len; i++) {
    const int shift = 8 * ((addr + i) % sizeof(T));
    bytes[i] = word >> shift;
  }
}

bool clint_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  if (len > 8)
    return false;

  increment(0);

  const auto msip_hart_id = (addr - MSIP_BASE) / sizeof(msip_t);
  const auto mtimecmp_hart_id = (addr - MTIMECMP_BASE) / sizeof(mtimecmp_t);

  if (addr >= MSIP_BASE && msip_hart_id < procs.size()) {
    if (len == 8) {
      // Implement double-word loads as a pair of word loads
      return load(addr, 4, bytes) && load(addr + 4, 4, bytes + 4);
    }

    msip_t res = !!(procs[msip_hart_id]->state.mip->read() & MIP_MSIP);
    partial_read(res, addr, len, bytes);
    return true;
  } else if (addr >= MTIMECMP_BASE && mtimecmp_hart_id < procs.size()) {
    partial_read(mtimecmp[mtimecmp_hart_id], addr, len, bytes);
  } else if (addr >= MTIME_BASE && addr < MTIME_BASE + sizeof(mtime_t)) {
    partial_read(mtime, addr, len, bytes);
  } else if (addr + len <= CLINT_SIZE) {
    memset(bytes, 0, len);
  } else {
    return false;
  }
  return true;
}

bool clint_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (len > 8)
    return false;

  const auto msip_hart_id = (addr - MSIP_BASE) / sizeof(msip_t);
  const auto mtimecmp_hart_id = (addr - MTIMECMP_BASE) / sizeof(mtimecmp_t);

  if (addr >= MSIP_BASE && msip_hart_id < procs.size()) {
    if (len == 8) {
      // Implement double-word stores as a pair of word stores
      return store(addr, 4, bytes) && store(addr + 4, 4, bytes + 4);
    }

    if (addr % sizeof(msip_t) == 0)
      procs[msip_hart_id]->state.mip->backdoor_write_with_mask(MIP_MSIP, bytes[0] & 1 ? MIP_MSIP : 0);
  } else if (addr >= MTIMECMP_BASE && mtimecmp_hart_id < procs.size()) {
    partial_write(&mtimecmp[mtimecmp_hart_id], addr, len, bytes);
  } else if (addr >= MTIME_BASE && addr < MTIME_BASE + sizeof(mtime_t)) {
    partial_write(&mtime, addr, len, bytes);
  } else if (addr + len <= CLINT_SIZE) {
    // Do nothing
  } else {
    return false;
  }
  increment(0);
  return true;
}

void clint_t::increment(reg_t inc)
{
  if (real_time) {
   struct timeval now;
   uint64_t diff_usecs;

   gettimeofday(&now, NULL);
   diff_usecs = ((now.tv_sec - real_time_ref_secs) * 1000000) + (now.tv_usec - real_time_ref_usecs);
   mtime = diff_usecs * freq_hz / 1000000;
  } else {
    mtime += inc;
  }
  for (size_t i = 0; i < procs.size(); i++) {
    procs[i]->state.time->sync(mtime);
    procs[i]->state.mip->backdoor_write_with_mask(MIP_MTIP, 0);
    if (mtime >= mtimecmp[i])
      procs[i]->state.mip->backdoor_write_with_mask(MIP_MTIP, MIP_MTIP);
  }
}
