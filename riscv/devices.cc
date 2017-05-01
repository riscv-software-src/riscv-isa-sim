#include "devices.h"

void bus_t::add_device(reg_t addr, abstract_device_t* dev)
{
  devices[-addr] = dev;
}

bool bus_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  auto it = devices.lower_bound(-addr);
  if (it == devices.end())
    return false;
  return it->second->load(addr - -it->first, len, bytes);
}

bool bus_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  auto it = devices.lower_bound(-addr);
  if (it == devices.end())
    return false;
  return it->second->store(addr - -it->first, len, bytes);
}

bus_t::descriptor bus_t::find_device(reg_t addr)
{
  auto it = devices.lower_bound(-addr);
  if (it == devices.end()) {
    bus_t::descriptor desc = {0, 0};
    return desc;
  }
  bus_t::descriptor desc = {-it->first, it->second};
  return desc;
}
