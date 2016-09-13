#include <cassert>

#include "debug_module.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"

bool debug_module_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr >= DEBUG_RAM_START && addr + len <= DEBUG_RAM_END) {
    memcpy(bytes, debug_ram + addr - DEBUG_RAM_START, len);
    return true;
  }

  if (addr >= DEBUG_ROM_START && addr + len <= DEBUG_ROM_END) {
    memcpy(bytes, debug_rom_raw + addr - DEBUG_ROM_START, len);
    return true;
  }

  fprintf(stderr, "ERROR: invalid load from debug module: %ld bytes at 0x%016"
          PRIx64 "\n", len, addr);
  return false;
}

bool debug_module_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr & (len-1)) {
    fprintf(stderr, "ERROR: unaligned store to debug module: %ld bytes at 0x%016"
            PRIx64 "\n", len, addr);
    return false;
  }

  if (addr >= DEBUG_RAM_START && addr + len <= DEBUG_RAM_END) {
    memcpy(debug_ram + addr - DEBUG_RAM_START, bytes, len);
    return true;
  } else if (len == 4 && addr == DEBUG_CLEARDEBINT) {
    clear_interrupt(bytes[0] | (bytes[1] << 8) |
        (bytes[2] << 16) | (bytes[3] << 24));
    return true;
  } else if (len == 4 && addr == DEBUG_SETHALTNOT) {
    set_halt_notification(bytes[0] | (bytes[1] << 8) |
        (bytes[2] << 16) | (bytes[3] << 24));
    return true;
  }

  fprintf(stderr, "ERROR: invalid store to debug module: %ld bytes at 0x%016"
          PRIx64 "\n", len, addr);
  return false;
}

void debug_module_t::ram_write32(unsigned int index, uint32_t value)
{
  char* base = debug_ram + index * 4;
  base[0] = value & 0xff;
  base[1] = (value >> 8) & 0xff;
  base[2] = (value >> 16) & 0xff;
  base[3] = (value >> 24) & 0xff;
}

uint32_t debug_module_t::ram_read32(unsigned int index)
{
  // It'd be better for raw_page (and all memory) to be unsigned chars, but mem
  // in sim_t is just chars, so I'm following that convention.
  unsigned char* base = (unsigned char*) (debug_ram + index * 4);
  uint32_t value = ((uint32_t) base[0]) |
    (((uint32_t) base[1]) << 8) |
    (((uint32_t) base[2]) << 16) |
    (((uint32_t) base[3]) << 24);
  return value;
}
