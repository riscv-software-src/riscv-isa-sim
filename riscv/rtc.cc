#include "devices.h"
#include "processor.h"

rtc_t::rtc_t(std::vector<processor_t*>& procs)
  : procs(procs), regs(1 + procs.size())
{
}

bool rtc_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  if (addr + len > size())
    return false;
  memcpy(bytes, (uint8_t*)&regs[0] + addr, len);
  return true;
}

bool rtc_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (addr + len > size() || addr < 8)
    return false;
  memcpy((uint8_t*)&regs[0] + addr, bytes, len);
  increment(0);
  return true;
}

void rtc_t::increment(reg_t inc)
{
  regs[0] += inc;
  for (size_t i = 0; i < procs.size(); i++) {
    procs[i]->state.mip &= ~MIP_MTIP;
    if (regs[0] >= regs[1+i])
      procs[i]->state.mip |= MIP_MTIP;
  }
}
