#include "dummy_debug.h"
#include "sim.h"
#include "mmu.h"

dummy_debug_t::~dummy_debug_t()
{

}

bool dummy_debug_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  // addr is internal addr!
  assert(addr < DM_BASE_ADDR);
  assert(addr + len <= DM_END_ADDR);
  int offset = addr / sizeof(uint8_t);
  memcpy(bytes, &dummy_debug_mem[offset], len);

  return true;
}

bool dummy_debug_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  // nothing is actually stored
  // because currently spike dm does not need to be working
  assert(addr < DM_BASE_ADDR);
  assert(addr + len <= DM_END_ADDR);
  return true;
}

// bool dummy_debug_t::update_dummy_mem(reg_t addr, size_t len, const uint8_t* bytes)
// {
//   assert(addr < DM_BASE_ADDR);
//   assert(addr + len <= DM_END_ADDR);
//   return memcpy((void *) (addr - DM_BASE_ADDR + dummy_debug_mem), bytes, len);
// }