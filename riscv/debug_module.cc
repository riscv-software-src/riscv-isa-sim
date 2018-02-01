#include <cassert>

#include "debug_module.h"
#include "debug_defines.h"
#include "opcodes.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"
#include "debug_rom/debug_rom_defines.h"

#if 0
#  define D(x) x
#else
#  define D(x)
#endif

///////////////////////// debug_module_t

debug_module_t::debug_module_t(sim_t *sim, unsigned progbufsize, unsigned max_bus_master_bits) :
  progbufsize(progbufsize),
  program_buffer_bytes(4 + 4*progbufsize),
  max_bus_master_bits(max_bus_master_bits),
  debug_progbuf_start(debug_data_start - program_buffer_bytes),
  debug_abstract_start(debug_progbuf_start - debug_abstract_size*4),
  sim(sim)
{
  program_buffer = new uint8_t[program_buffer_bytes];

  memset(halted, 0, sizeof(halted));
  memset(debug_rom_flags, 0, sizeof(debug_rom_flags));
  memset(resumeack, 0, sizeof(resumeack));
  memset(program_buffer, 0, program_buffer_bytes);
  program_buffer[4*progbufsize] = ebreak();
  program_buffer[4*progbufsize+1] = ebreak() >> 8;
  program_buffer[4*progbufsize+2] = ebreak() >> 16;
  program_buffer[4*progbufsize+3] = ebreak() >> 24;
  memset(dmdata, 0, sizeof(dmdata));

  write32(debug_rom_whereto, 0,
          jal(ZERO, debug_abstract_start - DEBUG_ROM_WHERETO));

  memset(debug_abstract, 0, sizeof(debug_abstract));

  reset();
}

debug_module_t::~debug_module_t()
{
  delete[] program_buffer;
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
  dmstatus.impebreak = true;
  dmstatus.authenticated = 1;
  dmstatus.version = 2;

  abstractcs = {0};
  abstractcs.datacount = sizeof(dmdata) / 4;
  abstractcs.progbufsize = progbufsize;

  abstractauto = {0};

  sbcs = {0};
  if (max_bus_master_bits > 0) {
    sbcs.version = 1;
    sbcs.asize = sizeof(reg_t) * 8;
  }
  if (max_bus_master_bits >= 64)
    sbcs.access64 = true;
  if (max_bus_master_bits >= 32)
    sbcs.access32 = true;
  if (max_bus_master_bits >= 16)
    sbcs.access16 = true;
  if (max_bus_master_bits >= 8)
    sbcs.access8 = true;
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

  if (addr >= debug_progbuf_start && ((addr + len) <= (debug_progbuf_start + program_buffer_bytes))) {
    memcpy(bytes, program_buffer + addr - debug_progbuf_start, len);
    return true;
  }

  fprintf(stderr, "ERROR: invalid load from debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr);

  return false;
}

bool debug_module_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  D(
      switch (len) {
        case 4:
          fprintf(stderr, "store(addr=0x%lx, len=%d, bytes=0x%08x); "
              "hartsel=0x%x\n", addr, (unsigned) len, *(uint32_t *) bytes,
              dmcontrol.hartsel);
          break;
        default:
          fprintf(stderr, "store(addr=0x%lx, len=%d, bytes=...); "
              "hartsel=0x%x\n", addr, (unsigned) len, dmcontrol.hartsel);
          break;
      }
   );

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

  if (addr >= debug_progbuf_start && ((addr + len) <= (debug_progbuf_start + program_buffer_bytes))) {
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

unsigned debug_module_t::sb_access_bits()
{
  return 8 << sbcs.sbaccess;
}

void debug_module_t::sb_autoincrement()
{
  if (!sbcs.autoincrement || !max_bus_master_bits)
    return;

  uint64_t value = sbaddress[0] + sb_access_bits() / 8;
  sbaddress[0] = value;
  uint32_t carry = value >> 32;

  value = sbaddress[1] + carry;
  sbaddress[1] = value;
  carry = value >> 32;

  value = sbaddress[2] + carry;
  sbaddress[2] = value;
  carry = value >> 32;

  sbaddress[3] += carry;
}

void debug_module_t::sb_read()
{
  reg_t address = ((uint64_t) sbaddress[1] << 32) | sbaddress[0];
  try {
    if (sbcs.sbaccess == 0 && max_bus_master_bits >= 8) {
      sbdata[0] = sim->debug_mmu->load_uint8(address);
    } else if (sbcs.sbaccess == 1 && max_bus_master_bits >= 16) {
      sbdata[0] = sim->debug_mmu->load_uint16(address);
    } else if (sbcs.sbaccess == 2 && max_bus_master_bits >= 32) {
      sbdata[0] = sim->debug_mmu->load_uint32(address);
    } else if (sbcs.sbaccess == 3 && max_bus_master_bits >= 64) {
      uint64_t value = sim->debug_mmu->load_uint32(address);
      sbdata[0] = value;
      sbdata[1] = value >> 32;
    } else {
      sbcs.error = 3;
    }
  } catch (trap_load_access_fault& t) {
    sbcs.error = 2;
  }
}

void debug_module_t::sb_write()
{
  reg_t address = ((uint64_t) sbaddress[1] << 32) | sbaddress[0];
  D(fprintf(stderr, "sb_write() 0x%x @ 0x%lx\n", sbdata[0], address));
  if (sbcs.sbaccess == 0 && max_bus_master_bits >= 8) {
    sim->debug_mmu->store_uint8(address, sbdata[0]);
  } else if (sbcs.sbaccess == 1 && max_bus_master_bits >= 16) {
    sim->debug_mmu->store_uint16(address, sbdata[0]);
  } else if (sbcs.sbaccess == 2 && max_bus_master_bits >= 32) {
    sim->debug_mmu->store_uint32(address, sbdata[0]);
  } else if (sbcs.sbaccess == 3 && max_bus_master_bits >= 64) {
    sim->debug_mmu->store_uint64(address,
        (((uint64_t) sbdata[1]) << 32) | sbdata[0]);
  } else {
    sbcs.error = 3;
  }
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
  } else if (address >= DMI_PROGBUF0 && address < DMI_PROGBUF0 + progbufsize) {
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
          result = set_field(result, ((1L<<hartsellen)-1) <<
              DMI_DMCONTROL_HARTSEL_OFFSET, dmcontrol.hartsel);
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

          result = set_field(result, DMI_DMSTATUS_IMPEBREAK,
              dmstatus.impebreak);
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
          result = set_field(result, DMI_DMSTATUS_VERSION, dmstatus.version);
        }
      	break;
      case DMI_ABSTRACTCS:
        result = set_field(result, DMI_ABSTRACTCS_CMDERR, abstractcs.cmderr);
        result = set_field(result, DMI_ABSTRACTCS_BUSY, abstractcs.busy);
        result = set_field(result, DMI_ABSTRACTCS_DATACOUNT, abstractcs.datacount);
        result = set_field(result, DMI_ABSTRACTCS_PROGBUFSIZE,
            abstractcs.progbufsize);
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
      case DMI_SBCS:
        result = set_field(result, DMI_SBCS_SBVERSION, sbcs.version);
        result = set_field(result, DMI_SBCS_SBREADONADDR, sbcs.readonaddr);
        result = set_field(result, DMI_SBCS_SBACCESS, sbcs.sbaccess);
        result = set_field(result, DMI_SBCS_SBAUTOINCREMENT, sbcs.autoincrement);
        result = set_field(result, DMI_SBCS_SBREADONDATA, sbcs.readondata);
        result = set_field(result, DMI_SBCS_SBERROR, sbcs.error);
        result = set_field(result, DMI_SBCS_SBASIZE, sbcs.asize);
        result = set_field(result, DMI_SBCS_SBACCESS128, sbcs.access128);
        result = set_field(result, DMI_SBCS_SBACCESS64, sbcs.access64);
        result = set_field(result, DMI_SBCS_SBACCESS32, sbcs.access32);
        result = set_field(result, DMI_SBCS_SBACCESS16, sbcs.access16);
        result = set_field(result, DMI_SBCS_SBACCESS8, sbcs.access8);
        break;
      case DMI_SBADDRESS0:
        result = sbaddress[0];
        break;
      case DMI_SBADDRESS1:
        result = sbaddress[1];
        break;
      case DMI_SBADDRESS2:
        result = sbaddress[2];
        break;
      case DMI_SBADDRESS3:
        result = sbaddress[3];
        break;
      case DMI_SBDATA0:
        result = sbdata[0];
        if (sbcs.error == 0) {
          sb_autoincrement();
          if (sbcs.readondata) {
            sb_read();
          }
        }
        break;
      case DMI_SBDATA1:
        result = sbdata[1];
        break;
      case DMI_SBDATA2:
        result = sbdata[2];
        break;
      case DMI_SBDATA3:
        result = sbdata[3];
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

  } else if (address >= DMI_PROGBUF0 && address < DMI_PROGBUF0 + progbufsize) {
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
          if (!dmcontrol.dmactive && get_field(value, DMI_DMCONTROL_DMACTIVE))
            reset();
          dmcontrol.dmactive = get_field(value, DMI_DMCONTROL_DMACTIVE);
          if (dmcontrol.dmactive) {
            dmcontrol.haltreq = get_field(value, DMI_DMCONTROL_HALTREQ);
            dmcontrol.resumereq = get_field(value, DMI_DMCONTROL_RESUMEREQ);
            dmcontrol.hartreset = get_field(value, DMI_DMCONTROL_HARTRESET);
            dmcontrol.ndmreset = get_field(value, DMI_DMCONTROL_NDMRESET);
            dmcontrol.hartsel = get_field(value, ((1L<<hartsellen)-1) <<
                DMI_DMCONTROL_HARTSEL_OFFSET);
          }
          processor_t *proc = current_proc();
          if (proc) {
            proc->halt_request = dmcontrol.haltreq;
            if (dmcontrol.resumereq) {
              debug_rom_flags[dmcontrol.hartsel] |= (1 << DEBUG_ROM_FLAG_RESUME);
              resumeack[dmcontrol.hartsel] = false;
            }
	    if (dmcontrol.hartreset) {
	      proc->reset();
	    }
          }
          if (dmcontrol.ndmreset) {
            for (size_t i = 0; i < sim->nprocs(); i++) {
              proc = sim->get_core(i);
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
      case DMI_SBCS:
        sbcs.readonaddr = get_field(value, DMI_SBCS_SBREADONADDR);
        sbcs.sbaccess = get_field(value, DMI_SBCS_SBACCESS);
        sbcs.autoincrement = get_field(value, DMI_SBCS_SBAUTOINCREMENT);
        sbcs.readondata = get_field(value, DMI_SBCS_SBREADONDATA);
        sbcs.error &= ~get_field(value, DMI_SBCS_SBERROR);
        return true;
      case DMI_SBADDRESS0:
        sbaddress[0] = value;
        if (sbcs.error == 0 && sbcs.readonaddr) {
          sb_read();
        }
        return true;
      case DMI_SBADDRESS1:
        sbaddress[1] = value;
        return true;
      case DMI_SBADDRESS2:
        sbaddress[2] = value;
        return true;
      case DMI_SBADDRESS3:
        sbaddress[3] = value;
        return true;
      case DMI_SBDATA0:
        sbdata[0] = value;
        if (sbcs.error == 0) {
          sb_write();
          if (sbcs.autoincrement && sbcs.error == 0) {
            sb_autoincrement();
          }
        }
        return true;
      case DMI_SBDATA1:
        sbdata[1] = value;
        return true;
      case DMI_SBDATA2:
        sbdata[2] = value;
        return true;
      case DMI_SBDATA3:
        sbdata[3] = value;
        return true;
    }
  }
  return false;
}
