#include <cassert>

#include "debug_module.h"
#include "debug_defines.h"
#include "opcodes.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"
#include "debug_rom/debug_rom_defines.h"

#if 1
#  define D(x) x
#else
#  define D(x)
#endif

///////////////////////// debug_module_t

debug_module_t::debug_module_t(sim_t *sim) : sim(sim)
{
  dmcontrol = {0};

  dmstatus = {0};
  dmstatus.authenticated = 1;
  dmstatus.versionlo = 2;

  abstractcs = {0};
  abstractcs.progsize = progsize;

  abstractauto = {0};

  memset(halted, 0, sizeof(halted));
  memset(debug_rom_flags, 0, sizeof(debug_rom_flags));
  memset(resumeack, 0, sizeof(resumeack));
  memset(program_buffer, 0, sizeof(program_buffer));
  memset(dmdata, 0, sizeof(dmdata));

  write32(debug_rom_whereto, 0,
          jal(ZERO, debug_abstract_start - DEBUG_ROM_WHERETO));

  memset(debug_abstract, 0, sizeof(debug_abstract));
 
}

void debug_module_t::reset()
{
  for (unsigned i = 0; i < sim->nprocs(); i++) {
    processor_t *proc = sim->get_core(i);
    if (proc)
      proc->halt_request = false;
  }

  dmcontrol = {0};

  dmstatus = {0};
  dmstatus.authenticated = 1;
  dmstatus.versionlo = 2;

  abstractcs = {0};
  abstractcs.datacount = sizeof(dmdata) / 4;
  abstractcs.progsize = progsize;

  abstractauto = {0};
}

void debug_module_t::add_device(bus_t *bus) {
  bus->add_device(DEBUG_START, this);
}

bool debug_module_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  addr = DEBUG_START + addr;

  if (addr >= DEBUG_ROM_ENTRY &&
      (addr + len) <= (DEBUG_ROM_ENTRY + debug_rom_raw_len)) {
    memcpy(bytes, debug_rom_raw + addr - DEBUG_ROM_ENTRY, len);
    return true;
  }

  if (addr >= DEBUG_ROM_WHERETO && (addr + len) <= (DEBUG_ROM_WHERETO + 4)) {
    memcpy(bytes, debug_rom_whereto + addr - DEBUG_ROM_WHERETO, len);
    return true;
  }

  if (addr >= DEBUG_ROM_FLAGS && ((addr + len) <= DEBUG_ROM_FLAGS + 1024)) {
    memcpy(bytes, debug_rom_flags + addr - DEBUG_ROM_FLAGS, len);
    return true;
  }

  if (addr >= debug_abstract_start && ((addr + len) <= (debug_abstract_start + sizeof(debug_abstract)))) {
    memcpy(bytes, debug_abstract + addr - debug_abstract_start, len);
    return true;
  }

  if (addr >= debug_data_start && (addr + len) <= (debug_data_start + sizeof(dmdata))) {
    memcpy(bytes, dmdata + addr - debug_data_start, len);
    return true;
  }
  
  if (addr >= debug_progbuf_start && ((addr + len) <= (debug_progbuf_start + sizeof(program_buffer)))) {
    memcpy(bytes, program_buffer + addr - debug_progbuf_start, len);
    return true;
  }

  fprintf(stderr, "ERROR: invalid load from debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);

  return false;
}

bool debug_module_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{

  uint8_t id_bytes[4];
  uint32_t id = 0;
  if (len == 4) {
    memcpy(id_bytes, bytes, 4);
    id = read32(id_bytes, 0);
  }

  addr = DEBUG_START + addr;
  
  if (addr >= debug_data_start && (addr + len) <= (debug_data_start + sizeof(dmdata))) {
    memcpy(dmdata + addr - debug_data_start, bytes, len);
    return true;
  }
  
  if (addr >= debug_progbuf_start && ((addr + len) <= (debug_progbuf_start + sizeof(program_buffer)))) {
    fprintf(stderr, "Successful write to program buffer %d bytes at %x\n", (int) len, (int) addr);
    memcpy(program_buffer + addr - debug_progbuf_start, bytes, len);
    
    return true;
  }

  if (addr == DEBUG_ROM_HALTED) {
    assert (len == 4);
    halted[id] = true;
    if (dmcontrol.hartsel == id) {
        if (0 == (debug_rom_flags[id] & (1 << DEBUG_ROM_FLAG_GO))){
          if (dmcontrol.hartsel == id) {
              abstractcs.busy = false;
          }
        }
    }
    return true;
  }

  if (addr == DEBUG_ROM_GOING) {
    debug_rom_flags[dmcontrol.hartsel] &= ~(1 << DEBUG_ROM_FLAG_GO);
    return true;
  }

  if (addr == DEBUG_ROM_RESUMING) {
    assert (len == 4);
    halted[id] = false;
    resumeack[id] = true;
    debug_rom_flags[id] &= ~(1 << DEBUG_ROM_FLAG_RESUME);
    return true;
  }

  if (addr == DEBUG_ROM_EXCEPTION) {
    if (abstractcs.cmderr == CMDERR_NONE) {
      abstractcs.cmderr = CMDERR_EXCEPTION;
    }
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
    result = read32(dmdata, i);
    if (abstractcs.busy) {
      result = -1;
      fprintf(stderr, "\ndmi_read(0x%02x (data[%d]) -> -1 because abstractcs.busy==true\n", address, i);
    }

    if (abstractcs.busy && abstractcs.cmderr == CMDERR_NONE) {
      abstractcs.cmderr = CMDERR_BUSY;
    }

    if (!abstractcs.busy && ((abstractauto.autoexecdata >> i) & 1)) {
      perform_abstract_command();
    }
  } else if (address >= DMI_PROGBUF0 && address < DMI_PROGBUF0 + progsize) {
    unsigned i = address - DMI_PROGBUF0;
    result = read32(program_buffer, i);
    if (abstractcs.busy) {
      result = -1;
      fprintf(stderr, "\ndmi_read(0x%02x (progbuf[%d]) -> -1 because abstractcs.busy==true\n", address, i);
    }
    if (!abstractcs.busy && ((abstractauto.autoexecprogbuf >> i) & 1)) {
      perform_abstract_command();
    }

  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          processor_t *proc = current_proc();
          if (proc)
            dmcontrol.haltreq = proc->halt_request;

          result = set_field(result, DMI_DMCONTROL_HALTREQ, dmcontrol.haltreq);
          result = set_field(result, DMI_DMCONTROL_RESUMEREQ, dmcontrol.resumereq);
          result = set_field(result, DMI_DMCONTROL_HARTSEL, dmcontrol.hartsel);
          result = set_field(result, DMI_DMCONTROL_HARTRESET, dmcontrol.hartreset);
	  result = set_field(result, DMI_DMCONTROL_NDMRESET, dmcontrol.ndmreset);
          result = set_field(result, DMI_DMCONTROL_DMACTIVE, dmcontrol.dmactive);
        }
        break;
      case DMI_DMSTATUS:
        {
          processor_t *proc = current_proc();

	  dmstatus.allnonexistant = false;
	  dmstatus.allunavail = false;
	  dmstatus.allrunning = false;
	  dmstatus.allhalted = false;
          dmstatus.allresumeack = false;
          if (proc) {
            if (halted[dmcontrol.hartsel]) {
              dmstatus.allhalted = true;
            } else {
              dmstatus.allrunning = true;
            }
          } else {
	    dmstatus.allnonexistant = true;
          }
	  dmstatus.anynonexistant = dmstatus.allnonexistant;
	  dmstatus.anyunavail = dmstatus.allunavail;
	  dmstatus.anyrunning = dmstatus.allrunning;
	  dmstatus.anyhalted = dmstatus.allhalted;
          if (proc) {
            if (resumeack[dmcontrol.hartsel]) {
              dmstatus.allresumeack = true;
            } else {
              dmstatus.allresumeack = false;
            }
          } else {
            dmstatus.allresumeack = false;
          }
          
	  result = set_field(result, DMI_DMSTATUS_ALLNONEXISTENT, dmstatus.allnonexistant);
	  result = set_field(result, DMI_DMSTATUS_ALLUNAVAIL, dmstatus.allunavail);
	  result = set_field(result, DMI_DMSTATUS_ALLRUNNING, dmstatus.allrunning);
	  result = set_field(result, DMI_DMSTATUS_ALLHALTED, dmstatus.allhalted);
          result = set_field(result, DMI_DMSTATUS_ALLRESUMEACK, dmstatus.allresumeack);
	  result = set_field(result, DMI_DMSTATUS_ANYNONEXISTENT, dmstatus.anynonexistant);
	  result = set_field(result, DMI_DMSTATUS_ANYUNAVAIL, dmstatus.anyunavail);
	  result = set_field(result, DMI_DMSTATUS_ANYRUNNING, dmstatus.anyrunning);
	  result = set_field(result, DMI_DMSTATUS_ANYHALTED, dmstatus.anyhalted);
          result = set_field(result, DMI_DMSTATUS_ANYRESUMEACK, dmstatus.anyresumeack);
          result = set_field(result, DMI_DMSTATUS_AUTHENTICATED, dmstatus.authenticated);
          result = set_field(result, DMI_DMSTATUS_AUTHBUSY, dmstatus.authbusy);
          result = set_field(result, DMI_DMSTATUS_VERSIONHI, dmstatus.versionhi);
          result = set_field(result, DMI_DMSTATUS_VERSIONLO, dmstatus.versionlo);
        }
      	break;
      case DMI_ABSTRACTCS:
        result = set_field(result, DMI_ABSTRACTCS_CMDERR, abstractcs.cmderr);
        result = set_field(result, DMI_ABSTRACTCS_BUSY, abstractcs.busy);
        result = set_field(result, DMI_ABSTRACTCS_DATACOUNT, abstractcs.datacount);
        result = set_field(result, DMI_ABSTRACTCS_PROGSIZE, abstractcs.progsize);
        break;
      case DMI_ABSTRACTAUTO:
        result = set_field(result, DMI_ABSTRACTAUTO_AUTOEXECPROGBUF, abstractauto.autoexecprogbuf);
        result = set_field(result, DMI_ABSTRACTAUTO_AUTOEXECDATA, abstractauto.autoexecdata);
        break;
      case DMI_COMMAND:
        result = 0;
        break;
      case DMI_HARTINFO:
        result = set_field(result, DMI_HARTINFO_NSCRATCH, 1);
        result = set_field(result, DMI_HARTINFO_DATAACCESS, 1);
        result = set_field(result, DMI_HARTINFO_DATASIZE, abstractcs.datacount);
        result = set_field(result, DMI_HARTINFO_DATAADDR, debug_data_start);
        break;
      default:
        result = 0;
        D(fprintf(stderr, "Unexpected. Returning Error."));
        return false;
    }
  }
  D(fprintf(stderr, "0x%x\n", result));
  *value = result;
  return true;
}

bool debug_module_t::perform_abstract_command()
{
  if (abstractcs.cmderr != CMDERR_NONE)
    return true;
  if (abstractcs.busy) {
    abstractcs.cmderr = CMDERR_BUSY;
    return true;
  }

  if ((command >> 24) == 0) {
    // register access
    unsigned size = get_field(command, AC_ACCESS_REGISTER_SIZE);
    bool write = get_field(command, AC_ACCESS_REGISTER_WRITE);
    unsigned regno = get_field(command, AC_ACCESS_REGISTER_REGNO);

    if (!halted[dmcontrol.hartsel]) {
      abstractcs.cmderr = CMDERR_HALTRESUME;
      return true;
    }

    if (get_field(command, AC_ACCESS_REGISTER_TRANSFER)) {

      if (regno < 0x1000 || regno >= 0x1020) {
        abstractcs.cmderr = CMDERR_NOTSUP;
        return true;
      }

      unsigned regnum = regno - 0x1000;

      switch (size) {
      case 2:
        if (write)
          write32(debug_abstract, 0, lw(regnum, ZERO, debug_data_start));
        else
          write32(debug_abstract, 0, sw(regnum, ZERO, debug_data_start));
        break;
      case 3:
        if (write)
          write32(debug_abstract, 0, ld(regnum, ZERO, debug_data_start));
        else
          write32(debug_abstract, 0, sd(regnum, ZERO, debug_data_start));
        break;
        /*
          case 4:
          if (write)
          write32(debug_rom_code, 0, lq(regnum, ZERO, debug_data_start));
          else
          write32(debug_rom_code, 0, sq(regnum, ZERO, debug_data_start));
          break;
        */
      default:
        abstractcs.cmderr = CMDERR_NOTSUP;
        return true;
      }
    } else {
      //NOP
      write32(debug_abstract, 0, addi(ZERO, ZERO, 0));
    }
    
    if (get_field(command, AC_ACCESS_REGISTER_POSTEXEC)) {
      // Since the next instruction is what we will use, just use nother NOP
      // to get there.
      write32(debug_abstract, 1, addi(ZERO, ZERO, 0));
    } else {
      write32(debug_abstract, 1, ebreak());
    }

    debug_rom_flags[dmcontrol.hartsel] |= 1 << DEBUG_ROM_FLAG_GO;
    
    abstractcs.busy = true;
  } else {
    abstractcs.cmderr = CMDERR_NOTSUP;
  }
  return true;
}

bool debug_module_t::dmi_write(unsigned address, uint32_t value)
{
  D(fprintf(stderr, "dmi_write(0x%x, 0x%x)\n", address, value));
  if (address >= DMI_DATA0 && address < DMI_DATA0 + abstractcs.datacount) {
    unsigned i = address - DMI_DATA0;
    if (!abstractcs.busy)
      write32(dmdata, address - DMI_DATA0, value);

    if (abstractcs.busy && abstractcs.cmderr == CMDERR_NONE) {
      abstractcs.cmderr = CMDERR_BUSY;
    }

    if (!abstractcs.busy && ((abstractauto.autoexecdata >> i) & 1)) {
      perform_abstract_command();
    }
    return true;

  } else if (address >= DMI_PROGBUF0 && address < DMI_PROGBUF0 + progsize) {
    unsigned i = address - DMI_PROGBUF0;
    
    if (!abstractcs.busy)
      write32(program_buffer, i, value);

    if (!abstractcs.busy && ((abstractauto.autoexecprogbuf >> i) & 1)) {
      perform_abstract_command();
    }
    return true;
    
  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          dmcontrol.dmactive = get_field(value, DMI_DMCONTROL_DMACTIVE);
          if (dmcontrol.dmactive) {
            dmcontrol.haltreq = get_field(value, DMI_DMCONTROL_HALTREQ);
            dmcontrol.resumereq = get_field(value, DMI_DMCONTROL_RESUMEREQ);
            dmcontrol.ndmreset = get_field(value, DMI_DMCONTROL_NDMRESET);
            dmcontrol.hartsel = get_field(value, DMI_DMCONTROL_HARTSEL);
          } else {
            reset();
          }
          processor_t *proc = current_proc();
          if (proc) {
            proc->halt_request = dmcontrol.haltreq;
            if (dmcontrol.resumereq) {
              debug_rom_flags[dmcontrol.hartsel] |= (1 << DEBUG_ROM_FLAG_RESUME);
              resumeack[dmcontrol.hartsel] = false;
            }
	    if (dmcontrol.ndmreset) {
	      proc->reset();
	    }
          }
        }
        return true;

      case DMI_COMMAND:
        command = value;
        return perform_abstract_command();

      case DMI_ABSTRACTCS:
        abstractcs.cmderr = (cmderr_t) (((uint32_t) (abstractcs.cmderr)) & (~(uint32_t)(get_field(value, DMI_ABSTRACTCS_CMDERR))));
        return true;

      case DMI_ABSTRACTAUTO:
        abstractauto.autoexecprogbuf = get_field(value,
            DMI_ABSTRACTAUTO_AUTOEXECPROGBUF);
        abstractauto.autoexecdata = get_field(value,
            DMI_ABSTRACTAUTO_AUTOEXECDATA);
        return true;
    }
  }
  return false;
}
