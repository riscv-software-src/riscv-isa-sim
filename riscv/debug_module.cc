#include <cassert>

#include "debug_module.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"

debug_module_t::debug_module_t()
{
  /* Copy Debug ROM into the page. */
  memcpy(raw_page + DEBUG_ROM_START - DEBUG_START,
      debug_rom_raw, debug_rom_raw_len);
}

bool debug_module_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  fprintf(stderr, "ERROR: invalid load from debug module: %ld bytes at 0x%lx\n",
      len, addr);
  return false;
}

bool debug_module_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr & (len-1)) {
    fprintf(stderr, "ERROR: unaligned store to debug module: %ld bytes at 0x%lx\n",
        len, addr);
    return false;
  }

  if (addr >= DEBUG_RAM_START && addr + len <= DEBUG_RAM_END) {
    memcpy(raw_page + addr - DEBUG_START, bytes, len);
    return true;
  } else if (len == 4 && addr == DEBUG_CLEARDEBINT) {
    clear_interrupt(bytes[4] | (bytes[5] << 8) |
        (bytes[6] << 16) | (bytes[7] << 24));
    return true;
  }

  fprintf(stderr, "ERROR: invalid store to debug module: %ld bytes at 0x%lx\n",
      len, addr);
  return false;
}

void debug_module_t::ram_write32(unsigned int index, uint32_t value)
{
  char* base = raw_page + DEBUG_RAM_START - DEBUG_START + index * 4;
  base[0] = value & 0xff;
  base[1] = (value >> 8) & 0xff;
  base[2] = (value >> 16) & 0xff;
  base[3] = (value >> 24) & 0xff;
}

char* debug_module_t::page(reg_t paddr)
{
  fprintf(stderr, "dm::page(0x%lx)\n", paddr);

  assert(PGSHIFT == 12);

  if (paddr == (DEBUG_START & PGMASK)) {
    return raw_page;
  }

  fprintf(stderr, "ERROR: invalid page to debug module at 0x%lx\n", paddr);
  return NULL;
}
