#include <sys/time.h>
#include "devices.h"
#include "processor.h"
#include "simif.h"

clint_t::clint_t(simif_t* sim, uint64_t freq_hz, bool real_time)
  : sim(sim), freq_hz(freq_hz), real_time(real_time), mtime(0)
{
  struct timeval base;

  gettimeofday(&base, NULL);

  real_time_ref_secs = base.tv_sec;
  real_time_ref_usecs = base.tv_usec;
  increment(0);
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

bool clint_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  if (len > 8)
    return false;

  increment(0);

  if (addr >= MSIP_BASE && addr < MTIMECMP_BASE) {
    if (len == 8) {
      // Implement double-word loads as a pair of word loads
      return load(addr, 4, bytes) && load(addr + 4, 4, bytes + 4);
    }

    const auto hart_id = (addr - MSIP_BASE) / sizeof(msip_t);
    const msip_t res = sim->get_harts().count(hart_id) && (sim->get_harts().at(hart_id)->state.mip->read() & MIP_MSIP);
    read_little_endian_reg(res, addr, len, bytes);
    return true;
  } else if (addr >= MTIMECMP_BASE && addr < MTIME_BASE) {
    const auto hart_id = (addr - MTIMECMP_BASE) / sizeof(mtimecmp_t);
    const mtime_t res = sim->get_harts().count(hart_id) ? mtimecmp[hart_id] : 0;
    read_little_endian_reg(res, addr, len, bytes);
  } else if (addr >= MTIME_BASE && addr < MTIME_BASE + sizeof(mtime_t)) {
    read_little_endian_reg(mtime, addr, len, bytes);
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

  if (addr >= MSIP_BASE && addr < MTIMECMP_BASE) {
    if (len == 8) {
      // Implement double-word stores as a pair of word stores
      return store(addr, 4, bytes) && store(addr + 4, 4, bytes + 4);
    }

    if (addr % sizeof(msip_t) == 0) {  // ignore in-between bytes
      msip_t msip = 0;
      write_little_endian_reg(&msip, addr, len, bytes);

      const auto hart_id = (addr - MSIP_BASE) / sizeof(msip_t);
      if (sim->get_harts().count(hart_id))
        sim->get_harts().at(hart_id)->state.mip->backdoor_write_with_mask(MIP_MSIP, msip & 1 ? MIP_MSIP : 0);
    }
  } else if (addr >= MTIMECMP_BASE && addr < MTIME_BASE) {
    const auto hart_id = (addr - MTIMECMP_BASE) / sizeof(mtimecmp_t);
    if (sim->get_harts().count(hart_id))
      write_little_endian_reg(&mtimecmp[hart_id], addr, len, bytes);
  } else if (addr >= MTIME_BASE && addr < MTIME_BASE + sizeof(mtime_t)) {
    write_little_endian_reg(&mtime, addr, len, bytes);
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

  for (const auto& [hart_id, hart] : sim->get_harts()) {
    hart->state.time->sync(mtime);
    hart->state.mip->backdoor_write_with_mask(MIP_MTIP, mtime >= mtimecmp[hart_id] ? MIP_MTIP : 0);
  }
}
