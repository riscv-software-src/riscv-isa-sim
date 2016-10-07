#include "devices.h"

void bus_t::add_device(reg_t addr, abstract_device_t* dev)
{
  devices[-addr] = dev;
}

bool bus_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  memset(bytes, 0, len);
  return true;
}

bool bus_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  return true;
}
