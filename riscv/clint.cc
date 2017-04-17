#include "devices.h"
#include "processor.h"

clint_t::clint_t(std::vector<processor_t*>& procs)
  : procs(procs), mtimecmp(procs.size())
{
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
  if (addr >= MSIP_BASE && addr + len <= MSIP_BASE + procs.size()*sizeof(msip_t)) {
    std::vector<msip_t> msip(procs.size());
    for (size_t i = 0; i < procs.size(); ++i)
      msip[i] = !!(procs[i]->state.mip & MIP_MSIP);
    memcpy(bytes, (uint8_t*)&msip[0] + addr - MSIP_BASE, len);
  } else if (addr >= MTIMECMP_BASE && addr + len <= MTIMECMP_BASE + procs.size()*sizeof(mtimecmp_t)) {
    memcpy(bytes, (uint8_t*)&mtimecmp[0] + addr - MTIMECMP_BASE, len);
  } else if (addr >= MTIME_BASE && addr + len <= MTIME_BASE + sizeof(mtime_t)) {
    memcpy(bytes, (uint8_t*)&mtime + addr - MTIME_BASE, len);
  } else {
    return false;
  }
  return true;
}

bool clint_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (addr >= MSIP_BASE && addr + len <= MSIP_BASE + procs.size()*sizeof(msip_t)) {
    std::vector<msip_t> msip(procs.size());
    std::vector<msip_t> mask(procs.size(), 0);
    memcpy((uint8_t*)&msip[0] + addr - MSIP_BASE, bytes, len);
    memset((uint8_t*)&mask[0] + addr - MSIP_BASE, 0xff, len);
    for (size_t i = 0; i < procs.size(); ++i) {
      if (!(mask[i] & 0xFF)) continue;
      procs[i]->state.mip &= ~MIP_MSIP;
      if (!!(msip[i] & 1))
        procs[i]->state.mip |= MIP_MSIP;
    }
  } else if (addr >= MTIMECMP_BASE && addr + len <= MTIMECMP_BASE + procs.size()*sizeof(mtimecmp_t)) {
    memcpy((uint8_t*)&mtimecmp[0] + addr - MTIMECMP_BASE, bytes, len);
  } else if (addr >= MTIME_BASE && addr + len <= MTIME_BASE + sizeof(mtime_t)) {
    memcpy((uint8_t*)&mtime + addr - MTIME_BASE, bytes, len);
  } else {
    return false;
  }
  increment(0);
  return true;
}

void clint_t::increment(reg_t inc)
{
  mtime += inc;
  for (size_t i = 0; i < procs.size(); i++) {
    procs[i]->state.mip &= ~MIP_MTIP;
    if (mtime >= mtimecmp[i])
      procs[i]->state.mip |= MIP_MTIP;
  }
}
