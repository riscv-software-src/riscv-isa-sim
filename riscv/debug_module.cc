#include <cassert>

#include "debug_module.h"
#include "debug_defines.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"

#if 1
#  define D(x) x
#else
#  define D(x)
#endif

debug_module_t::debug_module_t(sim_t *sim) :
  sim(sim),
  dmcontrol(1 << DMI_DMCONTROL_VERSION_OFFSET |
      1 << DMI_DMCONTROL_AUTHENTICATED_OFFSET),
  abstractcs(datacount << DMI_ABSTRACTCS_DATACOUNT_OFFSET)
{
}

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

  fprintf(stderr, "ERROR: invalid load from debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);
  return false;
}

bool debug_module_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr & (len-1)) {
    fprintf(stderr, "ERROR: unaligned store to debug module: %zd bytes at 0x%016"
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

  fprintf(stderr, "ERROR: invalid store to debug module: %zd bytes at 0x%016"
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

bool debug_module_t::dmi_read(unsigned address, uint32_t *value)
{
  D(fprintf(stderr, "dmi_read(0x%x) -> ", address));
  if (address >= DMI_DATA0 && address < DMI_DATA0 + datacount) {
    *value = data[address - DMI_DATA0];
  } else if (address >= DMI_IBUF0 && address < DMI_IBUF0 + progsize) {
    *value = ibuf[address - DMI_IBUF0];
  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          processor_t *proc = sim->get_core(get_field(dmcontrol,
                DMI_DMCONTROL_HARTSEL));
          if (proc) {
            D(fprintf(stderr, "(halted=%d) ", proc->halted()));
            if (proc->halted()) {
              dmcontrol = set_field(dmcontrol, DMI_DMCONTROL_HARTSTATUS, 0);
            } else {
              dmcontrol = set_field(dmcontrol, DMI_DMCONTROL_HARTSTATUS, 1);
            }
          } else {
            dmcontrol = set_field(dmcontrol, DMI_DMCONTROL_HARTSTATUS, 3);
          }
          *value = dmcontrol;
        }
        break;
      case DMI_ABSTRACTCS:
        *value = abstractcs;
        break;
      case DMI_ACCESSCS:
        *value = progsize << DMI_ACCESSCS_PROGSIZE_OFFSET;
        break;
      default:
        D(fprintf(stderr, "error\n"));
        return false;
    }
  }
  D(fprintf(stderr, "0x%x\n", *value));
  return true;
}

bool debug_module_t::dmi_write(unsigned address, uint32_t value)
{
  D(fprintf(stderr, "dmi_write(0x%x, 0x%x)\n", address, value));
  if (address >= DMI_DATA0 && address < DMI_DATA0 + datacount) {
    data[address - DMI_DATA0] = value;
    return true;
  } else if (address >= DMI_IBUF0 && address < DMI_IBUF0 + progsize) {
    ibuf[address - DMI_IBUF0] = value;
    return true;
  }
  return false;
}
