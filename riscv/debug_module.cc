#include <cassert>

#include "debug_module.h"
#include "debug_defines.h"
#include "opcodes.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"

#if 1
#  define D(x) x
#else
#  define D(x)
#endif

debug_module_t::debug_module_t(sim_t *sim) : sim(sim)
{
  dmcontrol.version = 1;

  write32(debug_rom_entry, 0, jal(0, 0));
}

void debug_module_t::reset()
{
  for (unsigned i = 0; i < sim->nprocs(); i++) {
    processor_t *proc = sim->get_core(i);
    if (proc)
      proc->halt_request = false;
  }

  dmcontrol.haltreq = 0;
  dmcontrol.reset = 0;
  dmcontrol.dmactive = 0;
  dmcontrol.hartsel = 0;
  dmcontrol.authenticated = 1;
  dmcontrol.version = 1;
  dmcontrol.authbusy = 0;
  dmcontrol.authtype = dmcontrol.AUTHTYPE_NOAUTH;
  abstractcs = datacount << DMI_ABSTRACTCS_DATACOUNT_OFFSET;
}

bool debug_module_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr >= DEBUG_ROM_ENTRY && addr <= DEBUG_ROM_CODE) {
    memcpy(bytes, debug_rom_entry + addr - DEBUG_ROM_ENTRY, len);
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

  // memcpy(debug_ram + addr - DEBUG_RAM_START, bytes, len);

  fprintf(stderr, "ERROR: invalid store to debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);
  return false;
}

void debug_module_t::write32(uint8_t *memory, unsigned int index, uint32_t value)
{
  uint8_t* base = memory + index * 4;
  base[0] = value & 0xff;
  base[1] = (value >> 8) & 0xff;
  base[2] = (value >> 16) & 0xff;
  base[3] = (value >> 24) & 0xff;
}

uint32_t debug_module_t::read32(uint8_t *memory, unsigned int index)
{
  uint8_t* base = memory + index * 4;
  uint32_t value = ((uint32_t) base[0]) |
    (((uint32_t) base[1]) << 8) |
    (((uint32_t) base[2]) << 16) |
    (((uint32_t) base[3]) << 24);
  return value;
}

processor_t *debug_module_t::current_proc() const
{
  processor_t *proc = NULL;
  try {
    proc = sim->get_core(dmcontrol.hartsel);
  } catch (const std::out_of_range&) {
  }
  return proc;
}

bool debug_module_t::dmi_read(unsigned address, uint32_t *value)
{
  uint32_t result = 0;
  D(fprintf(stderr, "dmi_read(0x%x) -> ", address));
  if (address >= DMI_DATA0 && address < DMI_DATA0 + datacount) {
    result = data[address - DMI_DATA0];
  } else if (address >= DMI_IBUF0 && address < DMI_IBUF0 + progsize) {
    result = ibuf[address - DMI_IBUF0];
  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          processor_t *proc = current_proc();
          if (proc) {
            if (proc->halted()) {
              dmcontrol.hartstatus = dmcontrol.HARTSTATUS_HALTED;
            } else {
              dmcontrol.hartstatus = dmcontrol.HARTSTATUS_RUNNING;
            }
            dmcontrol.haltreq = proc->halt_request;
          } else {
            dmcontrol.hartstatus = dmcontrol.HARTSTATUS_NOTEXIST;
          }
          result = set_field(result, DMI_DMCONTROL_HALTREQ, dmcontrol.haltreq);
          result = set_field(result, DMI_DMCONTROL_RESET, dmcontrol.reset);
          result = set_field(result, DMI_DMCONTROL_DMACTIVE, dmcontrol.dmactive);
          result = set_field(result, DMI_DMCONTROL_HARTSTATUS, dmcontrol.hartstatus);
          result = set_field(result, DMI_DMCONTROL_HARTSEL, dmcontrol.hartsel);
          result = set_field(result, DMI_DMCONTROL_AUTHENTICATED, dmcontrol.authenticated);
          result = set_field(result, DMI_DMCONTROL_AUTHBUSY, dmcontrol.authbusy);
          result = set_field(result, DMI_DMCONTROL_AUTHTYPE, dmcontrol.authtype);
          result = set_field(result, DMI_DMCONTROL_VERSION, dmcontrol.version);
        }
        break;
      case DMI_ABSTRACTCS:
        result = abstractcs;
        break;
      case DMI_ACCESSCS:
        result = progsize << DMI_ACCESSCS_PROGSIZE_OFFSET;
        break;
      default:
        D(fprintf(stderr, "error\n"));
        return false;
    }
  }
  D(fprintf(stderr, "0x%x\n", result));
  *value = result;
  return true;
}

bool debug_module_t::perform_abstract_command(uint32_t command)
{
  return false;
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
  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          dmcontrol.dmactive = get_field(value, DMI_DMCONTROL_DMACTIVE);
          if (dmcontrol.dmactive) {
            dmcontrol.haltreq = get_field(value, DMI_DMCONTROL_HALTREQ);
            dmcontrol.reset = get_field(value, DMI_DMCONTROL_RESET);
            dmcontrol.hartsel = get_field(value, DMI_DMCONTROL_HARTSEL);
          } else {
            reset();
          }
          processor_t *proc = current_proc();
          if (proc) {
            proc->halt_request = dmcontrol.haltreq;
          }
        }
        return true;

      case DMI_ABSTRACTCS:
        return perform_abstract_command(value);
    }
  }
  return false;
}
