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

///////////////////////// debug_module_data_t

debug_module_data_t::debug_module_data_t()
{
  memset(data, 0, sizeof(data));
}

bool debug_module_data_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  D(fprintf(stderr, "debug_module_data_t load 0x%lx bytes at 0x%lx\n", len,
        addr));

  if (addr + len < sizeof(data)) {
    memcpy(bytes, data + addr, len);
    return true;
  }

  fprintf(stderr, "ERROR: invalid load from debug_module_data_t: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);

  return false;
}

bool debug_module_data_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  D(fprintf(stderr, "debug_module_data_t store 0x%lx bytes at 0x%lx\n", len,
        addr));

  if (addr + len < sizeof(data)) {
    memcpy(data + addr, bytes, len);
    return true;
  }

  fprintf(stderr, "ERROR: invalid store to debug_module_data_t: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);
  return false;
}

uint32_t debug_module_data_t::read32(reg_t addr) const
{
  assert(addr + 4 <= sizeof(data));
  return data[addr] |
    (data[addr + 1] << 8) |
    (data[addr + 2] << 16) |
    (data[addr + 3] << 24);
}

void debug_module_data_t::write32(reg_t addr, uint32_t value)
{
  fprintf(stderr, "debug_module_data_t::write32(0x%lx, 0x%x)\n", addr, value);
  assert(addr + 4 <= sizeof(data));
  data[addr] = value & 0xff;
  data[addr + 1] = (value >> 8) & 0xff;
  data[addr + 2] = (value >> 16) & 0xff;
  data[addr + 3] = (value >> 24) & 0xff;
}

///////////////////////// debug_module_t

debug_module_t::debug_module_t(sim_t *sim) : sim(sim),
  next_action(jal(ZERO, 0)),
  action_executed(false)
{
  dmcontrol = {0};
  dmcontrol.version = 1;

  for (unsigned i = 0; i < DEBUG_ROM_ENTRY_SIZE / 4; i++) {
    write32(debug_rom_entry, i, jal(ZERO, 0));
    halted[i] = false;
  }

  memset(program_buffer, 0, sizeof(program_buffer));
}

void debug_module_t::reset()
{
  for (unsigned i = 0; i < sim->nprocs(); i++) {
    processor_t *proc = sim->get_core(i);
    if (proc)
      proc->halt_request = false;
  }

  dmcontrol = {0};
  dmcontrol.authenticated = 1;
  dmcontrol.version = 1;
  dmcontrol.authtype = dmcontrol.AUTHTYPE_NOAUTH;

  abstractcs = {0};
  abstractcs.datacount = sizeof(dmdata.data) / 4;
}

void debug_module_t::add_device(bus_t *bus) {
  bus->add_device(DEBUG_START, this);
  bus->add_device(DEBUG_EXCHANGE, &dmdata);
}

bool debug_module_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  D(fprintf(stderr, "debug_module_t load 0x%lx bytes at 0x%lx\n",
        len, addr));
  addr = DEBUG_START + addr;

  if (addr >= DEBUG_ROM_ENTRY &&
      addr < DEBUG_ROM_ENTRY + DEBUG_ROM_ENTRY_SIZE) {

    if (read32(debug_rom_entry, dmcontrol.hartsel) == jal(ZERO, 0)) {
      // We're here in an infinite loop. That means that whatever abstract
      // command has complete.
      abstractcs.busy = false;
    }

    action_executed = true;

    halted[(addr - DEBUG_ROM_ENTRY) / 4] = true;
    memcpy(bytes, debug_rom_entry + addr - DEBUG_ROM_ENTRY, len);
    return true;
  }

  if (action_executed) {
    // Restore the jump-to-self loop.
    write32(debug_rom_entry, dmcontrol.hartsel, next_action);
    next_action = jal(ZERO, 0);
    action_executed = false;
  }

  if (addr >= DEBUG_ROM_CODE &&
      addr < DEBUG_ROM_CODE + DEBUG_ROM_CODE_SIZE) {

    if (read32(debug_rom_code, 0) == dret()) {
      abstractcs.busy = false;
      halted[dmcontrol.hartsel] = false;
    }

    memcpy(bytes, debug_rom_code + addr - DEBUG_ROM_CODE, len);
    return true;
  }

  if (addr >= DEBUG_RAM_START && addr < DEBUG_RAM_END) {
    memcpy(bytes, program_buffer + addr - DEBUG_RAM_START, len);
    return true;
  }

  if (addr >= DEBUG_ROM_EXCEPTION &&
      addr < DEBUG_ROM_EXCEPTION + DEBUG_ROM_EXCEPTION_SIZE) {
    memcpy(bytes, debug_rom_exception + addr - DEBUG_ROM_EXCEPTION, len);
    if (abstractcs.cmderr == abstractcs.CMDERR_NONE) {
      abstractcs.cmderr = abstractcs.CMDERR_EXCEPTION;
    }
    return true;
  }

  fprintf(stderr, "ERROR: invalid load from debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);

  return false;
}

bool debug_module_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr >= DEBUG_RAM_START && addr < DEBUG_RAM_END) {
    memcpy(program_buffer + addr - DEBUG_RAM_START, bytes, len);
    return true;
  }

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
  if (address >= DMI_DATA0 && address < DMI_DATA0 + abstractcs.datacount) {
    unsigned i = address - DMI_DATA0;
    result = dmdata.read32(4 * i);

    bool autoexec = false;
    switch (i) {
      case 0:   autoexec = abstractcs.autoexec0; break;
      case 1:   autoexec = abstractcs.autoexec1; break;
      case 2:   autoexec = abstractcs.autoexec2; break;
      case 3:   autoexec = abstractcs.autoexec3; break;
      case 4:   autoexec = abstractcs.autoexec4; break;
      case 5:   autoexec = abstractcs.autoexec5; break;
      case 6:   autoexec = abstractcs.autoexec6; break;
      case 7:   autoexec = abstractcs.autoexec7; break;
    }
    if (autoexec) {
      perform_abstract_command();
    }
  } else if (address >= DMI_IBUF0 && address < DMI_IBUF0 + progsize) {
    result = read32(program_buffer, address - DMI_IBUF0);
  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          processor_t *proc = current_proc();
          if (proc) {
            if (halted[dmcontrol.hartsel]) {
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
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC7, abstractcs.autoexec7);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC6, abstractcs.autoexec6);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC5, abstractcs.autoexec5);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC4, abstractcs.autoexec4);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC3, abstractcs.autoexec3);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC2, abstractcs.autoexec2);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC1, abstractcs.autoexec1);
        result = set_field(result, DMI_ABSTRACTCS_AUTOEXEC0, abstractcs.autoexec0);
        result = set_field(result, DMI_ABSTRACTCS_CMDERR, abstractcs.cmderr);
        result = set_field(result, DMI_ABSTRACTCS_BUSY, abstractcs.busy);
        result = set_field(result, DMI_ABSTRACTCS_DATACOUNT, abstractcs.datacount);
        break;
      case DMI_ACCESSCS:
        result = progsize << DMI_ACCESSCS_PROGSIZE_OFFSET;
        break;
      case DMI_COMMAND:
        result = 0;
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

bool debug_module_t::perform_abstract_command()
{
  if (abstractcs.cmderr != abstractcs.CMDERR_NONE)
    return true;
  if (abstractcs.busy) {
    abstractcs.cmderr = abstractcs.CMDERR_BUSY;
    return true;
  }

  if ((command >> 24) == 0) {
    // register access
    unsigned size = get_field(command, AC_ACCESS_REGISTER_SIZE);
    bool write = get_field(command, AC_ACCESS_REGISTER_WRITE);
    unsigned regno = get_field(command, AC_ACCESS_REGISTER_REGNO);

    if (regno < 0x1000 || regno >= 0x1020) {
      abstractcs.cmderr = abstractcs.CMDERR_NOTSUP;
      return true;
    }

    unsigned regnum = regno - 0x1000;

    if (!halted[dmcontrol.hartsel]) {
      abstractcs.cmderr = abstractcs.CMDERR_HALTRESUME;
      return true;
    }

    switch (size) {
      case 2:
        if (write)
          write32(debug_rom_code, 0, lw(regnum, ZERO, DEBUG_EXCHANGE));
        else
          write32(debug_rom_code, 0, sw(regnum, ZERO, DEBUG_EXCHANGE));
        break;
      case 3:
        if (write)
          write32(debug_rom_code, 0, ld(regnum, ZERO, DEBUG_EXCHANGE));
        else
          write32(debug_rom_code, 0, sd(regnum, ZERO, DEBUG_EXCHANGE));
        break;
      /*
      case 4:
        if (write)
          write32(debug_rom_code, 0, lq(regnum, ZERO, DEBUG_EXCHANGE));
        else
          write32(debug_rom_code, 0, sq(regnum, ZERO, DEBUG_EXCHANGE));
        break;
        */
      default:
        abstractcs.cmderr = abstractcs.CMDERR_NOTSUP;
        return true;
    }
    if (get_field(command, AC_ACCESS_REGISTER_POSTEXEC)) {
      write32(debug_rom_code, 1, jal(ZERO, DEBUG_RAM_START - DEBUG_ROM_CODE - 4));
    } else {
      write32(debug_rom_code, 1, ebreak());
    }

    if (get_field(command, AC_ACCESS_REGISTER_PREEXEC)) {
      write32(debug_rom_entry, dmcontrol.hartsel,
          jal(ZERO, DEBUG_RAM_START - (DEBUG_ROM_ENTRY + 4 * dmcontrol.hartsel)));
      next_action =
        jal(ZERO, DEBUG_ROM_CODE - (DEBUG_ROM_ENTRY + 4 * dmcontrol.hartsel));
    } else {
      write32(debug_rom_entry, dmcontrol.hartsel,
          jal(ZERO, DEBUG_ROM_CODE - (DEBUG_ROM_ENTRY + 4 * dmcontrol.hartsel)));
    }

    write32(debug_rom_exception, dmcontrol.hartsel,
        jal(ZERO, (DEBUG_ROM_ENTRY + 4 * dmcontrol.hartsel) - DEBUG_ROM_EXCEPTION));
    abstractcs.busy = true;
  } else {
    abstractcs.cmderr = abstractcs.CMDERR_NOTSUP;
  }
  return true;
}

bool debug_module_t::dmi_write(unsigned address, uint32_t value)
{
  D(fprintf(stderr, "dmi_write(0x%x, 0x%x)\n", address, value));
  if (address >= DMI_DATA0 && address < DMI_DATA0 + abstractcs.datacount) {
    unsigned i = address - DMI_DATA0;
    dmdata.write32(4 * i, value);

    bool autoexec = false;
    switch (i) {
      case 0:   autoexec = abstractcs.autoexec0; break;
      case 1:   autoexec = abstractcs.autoexec1; break;
      case 2:   autoexec = abstractcs.autoexec2; break;
      case 3:   autoexec = abstractcs.autoexec3; break;
      case 4:   autoexec = abstractcs.autoexec4; break;
      case 5:   autoexec = abstractcs.autoexec5; break;
      case 6:   autoexec = abstractcs.autoexec6; break;
      case 7:   autoexec = abstractcs.autoexec7; break;
    }
    if (autoexec) {
      perform_abstract_command();
    }
    return true;

  } else if (address >= DMI_IBUF0 && address < DMI_IBUF0 + progsize) {
    write32(program_buffer, address - DMI_IBUF0, value);
    return true;
  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          dmcontrol.dmactive = get_field(value, DMI_DMCONTROL_DMACTIVE);
          if (dmcontrol.dmactive) {
            dmcontrol.haltreq = get_field(value, DMI_DMCONTROL_HALTREQ);
            dmcontrol.resumereq = get_field(value, DMI_DMCONTROL_RESUMEREQ);
            dmcontrol.reset = get_field(value, DMI_DMCONTROL_RESET);
            dmcontrol.hartsel = get_field(value, DMI_DMCONTROL_HARTSEL);
          } else {
            reset();
          }
          processor_t *proc = current_proc();
          if (proc) {
            proc->halt_request = dmcontrol.haltreq;
            if (dmcontrol.resumereq) {
              write32(debug_rom_code, 0, dret());
              write32(debug_rom_entry, dmcontrol.hartsel,
                  jal(ZERO, DEBUG_ROM_CODE - (DEBUG_ROM_ENTRY + 4 * dmcontrol.hartsel)));
              abstractcs.busy = true;
            }
          }
        }
        return true;

      case DMI_COMMAND:
        command = value;
        return perform_abstract_command();

      case DMI_ABSTRACTCS:
        abstractcs.autoexec7 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC7);
        abstractcs.autoexec6 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC6);
        abstractcs.autoexec5 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC5);
        abstractcs.autoexec4 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC4);
        abstractcs.autoexec3 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC3);
        abstractcs.autoexec2 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC2);
        abstractcs.autoexec1 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC1);
        abstractcs.autoexec0 = get_field(value, DMI_ABSTRACTCS_AUTOEXEC0);
        if (get_field(value, DMI_ABSTRACTCS_CMDERR) == abstractcs.CMDERR_NONE) {
          abstractcs.cmderr = abstractcs.CMDERR_NONE;
        }
        return true;
    }
  }
  return false;
}
