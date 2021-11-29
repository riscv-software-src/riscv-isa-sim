#include <cassert>

#include "sim.h"
#include "debug_module.h"
#include "debug_defines.h"
#include "opcodes.h"
#include "mmu.h"

#include "debug_rom/debug_rom.h"
#include "debug_rom_defines.h"

#if 0
#  define D(x) x
#else
#  define D(x)
#endif

// Return the number of bits wide that a field has to be to encode up to n
// different values.
// 1->0, 2->1, 3->2, 4->2
static unsigned field_width(unsigned n)
{
  unsigned i = 0;
  n -= 1;
  while (n) {
    i++;
    n >>= 1;
  }
  return i;
}

///////////////////////// debug_module_t

debug_module_t::debug_module_t(sim_t *sim, const debug_module_config_t &config) :
  nprocs(sim->nprocs()),
  config(config),
  program_buffer_bytes((config.support_impebreak ? 4 : 0) + 4*config.progbufsize),
  debug_progbuf_start(debug_data_start - program_buffer_bytes),
  debug_abstract_start(debug_progbuf_start - debug_abstract_size*4),
  custom_base(0),
  hartsellen(field_width(sim->nprocs())),
  sim(sim),
  // The spec lets a debugger select nonexistent harts. Create hart_state for
  // them because I'm too lazy to add the code to just ignore accesses.
  hart_state(1 << field_width(sim->nprocs())),
  hart_array_mask(sim->nprocs()),
  rti_remaining(0)
{
  D(fprintf(stderr, "debug_data_start=0x%x\n", debug_data_start));
  D(fprintf(stderr, "debug_progbuf_start=0x%x\n", debug_progbuf_start));
  D(fprintf(stderr, "debug_abstract_start=0x%x\n", debug_abstract_start));

  assert(nprocs <= 1024);

  program_buffer = new uint8_t[program_buffer_bytes];

  memset(debug_rom_flags, 0, sizeof(debug_rom_flags));
  memset(program_buffer, 0, program_buffer_bytes);
  memset(dmdata, 0, sizeof(dmdata));

  if (config.support_impebreak) {
    program_buffer[4*config.progbufsize] = ebreak();
    program_buffer[4*config.progbufsize+1] = ebreak() >> 8;
    program_buffer[4*config.progbufsize+2] = ebreak() >> 16;
    program_buffer[4*config.progbufsize+3] = ebreak() >> 24;
  }

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
  assert(sim->nprocs() > 0);
  for (unsigned i = 0; i < sim->nprocs(); i++) {
    processor_t *proc = sim->get_core(i);
    if (proc)
      proc->halt_request = proc->HR_NONE;
  }

  dmcontrol = {0};

  dmstatus = {0};
  dmstatus.impebreak = config.support_impebreak;
  dmstatus.authenticated = !config.require_authentication;
  dmstatus.version = 2;

  abstractcs = {0};
  abstractcs.datacount = sizeof(dmdata) / 4;
  abstractcs.progbufsize = config.progbufsize;

  abstractauto = {0};

  sbcs = {0};
  if (config.max_bus_master_bits > 0) {
    sbcs.version = 1;
    sbcs.asize = sizeof(reg_t) * 8;
  }
  if (config.max_bus_master_bits >= 64)
    sbcs.access64 = true;
  if (config.max_bus_master_bits >= 32)
    sbcs.access32 = true;
  if (config.max_bus_master_bits >= 16)
    sbcs.access16 = true;
  if (config.max_bus_master_bits >= 8)
    sbcs.access8 = true;

  challenge = random();
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

  D(fprintf(stderr, "ERROR: invalid load from debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr));

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
    if (!hart_state[id].halted) {
      hart_state[id].halted = true;
      if (hart_state[id].haltgroup) {
        for (unsigned i = 0; i < nprocs; i++) {
          if (!hart_state[i].halted &&
              hart_state[i].haltgroup == hart_state[id].haltgroup) {
            processor_t *proc = sim->get_core(i);
            proc->halt_request = proc->HR_GROUP;
            // TODO: What if the debugger comes and writes dmcontrol before the
            // halt occurs?
          }
        }
      }
    }
    if (dmcontrol.hartsel == id) {
        if (0 == (debug_rom_flags[id] & (1 << DEBUG_ROM_FLAG_GO))){
          if (dmcontrol.hartsel == id) {
              abstract_command_completed = true;
          }
        }
    }
    return true;
  }

  if (addr == DEBUG_ROM_GOING) {
    assert(len == 4);
    debug_rom_flags[id] &= ~(1 << DEBUG_ROM_FLAG_GO);
    return true;
  }

  if (addr == DEBUG_ROM_RESUMING) {
    assert (len == 4);
    hart_state[id].halted = false;
    hart_state[id].resumeack = true;
    debug_rom_flags[id] &= ~(1 << DEBUG_ROM_FLAG_RESUME);
    return true;
  }

  if (addr == DEBUG_ROM_EXCEPTION) {
    if (abstractcs.cmderr == CMDERR_NONE) {
      abstractcs.cmderr = CMDERR_EXCEPTION;
    }
    return true;
  }

  D(fprintf(stderr, "ERROR: invalid store to debug module: %zd bytes at 0x%016"
          PRIx64 "\n", len, addr));
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

processor_t *debug_module_t::processor(unsigned hartid) const
{
  processor_t *proc = NULL;
  try {
    proc = sim->get_core(hartid);
  } catch (const std::out_of_range&) {
  }
  return proc;
}

bool debug_module_t::hart_selected(unsigned hartid) const
{
  if (dmcontrol.hasel) {
    return hartid == dmcontrol.hartsel || hart_array_mask[hartid];
  } else {
    return hartid == dmcontrol.hartsel;
  }
}

unsigned debug_module_t::sb_access_bits()
{
  return 8 << sbcs.sbaccess;
}

void debug_module_t::sb_autoincrement()
{
  if (!sbcs.autoincrement || !config.max_bus_master_bits)
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
    if (sbcs.sbaccess == 0 && config.max_bus_master_bits >= 8) {
      sbdata[0] = sim->debug_mmu->load_uint8(address);
    } else if (sbcs.sbaccess == 1 && config.max_bus_master_bits >= 16) {
      sbdata[0] = sim->debug_mmu->load_uint16(address);
    } else if (sbcs.sbaccess == 2 && config.max_bus_master_bits >= 32) {
      sbdata[0] = sim->debug_mmu->load_uint32(address);
    } else if (sbcs.sbaccess == 3 && config.max_bus_master_bits >= 64) {
      uint64_t value = sim->debug_mmu->load_uint64(address);
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
  if (sbcs.sbaccess == 0 && config.max_bus_master_bits >= 8) {
    sim->debug_mmu->store_uint8(address, sbdata[0]);
  } else if (sbcs.sbaccess == 1 && config.max_bus_master_bits >= 16) {
    sim->debug_mmu->store_uint16(address, sbdata[0]);
  } else if (sbcs.sbaccess == 2 && config.max_bus_master_bits >= 32) {
    sim->debug_mmu->store_uint32(address, sbdata[0]);
  } else if (sbcs.sbaccess == 3 && config.max_bus_master_bits >= 64) {
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
      D(fprintf(stderr, "\ndmi_read(0x%02x (data[%d]) -> -1 because abstractcs.busy==true\n", address, i));
    }

    if (abstractcs.busy && abstractcs.cmderr == CMDERR_NONE) {
      abstractcs.cmderr = CMDERR_BUSY;
    }

    if (!abstractcs.busy && ((abstractauto.autoexecdata >> i) & 1)) {
      perform_abstract_command();
    }
  } else if (address >= DMI_PROGBUF0 && address < DMI_PROGBUF0 + config.progbufsize) {
    unsigned i = address - DMI_PROGBUF0;
    result = read32(program_buffer, i);
    if (abstractcs.busy) {
      result = -1;
      D(fprintf(stderr, "\ndmi_read(0x%02x (progbuf[%d]) -> -1 because abstractcs.busy==true\n", address, i));
    }
    if (!abstractcs.busy && ((abstractauto.autoexecprogbuf >> i) & 1)) {
      perform_abstract_command();
    }

  } else {
    switch (address) {
      case DMI_DMCONTROL:
        {
          result = set_field(result, DMI_DMCONTROL_HALTREQ, dmcontrol.haltreq);
          result = set_field(result, DMI_DMCONTROL_RESUMEREQ, dmcontrol.resumereq);
          result = set_field(result, DMI_DMCONTROL_HARTSELHI,
              dmcontrol.hartsel >> DMI_DMCONTROL_HARTSELLO_LENGTH);
          result = set_field(result, DMI_DMCONTROL_HASEL, dmcontrol.hasel);
          result = set_field(result, DMI_DMCONTROL_HARTSELLO, dmcontrol.hartsel);
          result = set_field(result, DMI_DMCONTROL_HARTRESET, dmcontrol.hartreset);
	  result = set_field(result, DMI_DMCONTROL_NDMRESET, dmcontrol.ndmreset);
          result = set_field(result, DMI_DMCONTROL_DMACTIVE, dmcontrol.dmactive);
        }
        break;
      case DMI_DMSTATUS:
        {
	  dmstatus.allhalted = true;
          dmstatus.anyhalted = false;
	  dmstatus.allrunning = true;
          dmstatus.anyrunning = false;
          dmstatus.allnonexistant = true;
          dmstatus.allresumeack = true;
          dmstatus.anyresumeack = false;
          for (unsigned i = 0; i < nprocs; i++) {
            if (hart_selected(i)) {
              dmstatus.allnonexistant = false;
              if (hart_state[i].resumeack) {
                dmstatus.anyresumeack = true;
              } else {
                dmstatus.allresumeack = false;
              }
              if (hart_state[i].halted) {
                dmstatus.allrunning = false;
                dmstatus.anyhalted = true;
              } else {
                dmstatus.allhalted = false;
                dmstatus.anyrunning = true;
              }
            }
          }

          // We don't allow selecting non-existant harts through
          // hart_array_mask, so the only way it's possible is by writing a
          // non-existant hartsel.
          dmstatus.anynonexistant = (dmcontrol.hartsel >= nprocs);

	  dmstatus.allunavail = false;
	  dmstatus.anyunavail = false;

          result = set_field(result, DMI_DMSTATUS_IMPEBREAK,
              dmstatus.impebreak);
          result = set_field(result, DMI_DMSTATUS_ALLHAVERESET,
              hart_state[dmcontrol.hartsel].havereset);
          result = set_field(result, DMI_DMSTATUS_ANYHAVERESET,
              hart_state[dmcontrol.hartsel].havereset);
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
      case DMI_HAWINDOWSEL:
        result = hawindowsel;
        break;
      case DMI_HAWINDOW:
        {
          unsigned base = hawindowsel * 32;
          for (unsigned i = 0; i < 32; i++) {
            unsigned n = base + i;
            if (n < nprocs && hart_array_mask[n]) {
              result |= 1 << i;
            }
          }
        }
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
          if (sbcs.readondata) {
            sb_read();
          }
          if (sbcs.error == 0) {
            sb_autoincrement();
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
      case DMI_AUTHDATA:
        result = challenge;
        break;
      case DMI_DMCS2:
        result = set_field(result, DMI_DMCS2_HALTGROUP,
            hart_state[dmcontrol.hartsel].haltgroup);
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

void debug_module_t::run_test_idle()
{
  if (rti_remaining > 0) {
    rti_remaining--;
  }
  if (rti_remaining == 0 && abstractcs.busy && abstract_command_completed) {
    abstractcs.busy = false;
  }
}

static bool is_fpu_reg(unsigned regno)
{
  return (regno >= 0x1020 && regno <= 0x103f) || regno == CSR_FFLAGS ||
    regno == CSR_FRM || regno == CSR_FCSR;
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
    unsigned size = get_field(command, AC_ACCESS_REGISTER_AARSIZE);
    bool write = get_field(command, AC_ACCESS_REGISTER_WRITE);
    unsigned regno = get_field(command, AC_ACCESS_REGISTER_REGNO);

    if (!hart_state[dmcontrol.hartsel].halted) {
      abstractcs.cmderr = CMDERR_HALTRESUME;
      return true;
    }

    unsigned i = 0;
    if (get_field(command, AC_ACCESS_REGISTER_TRANSFER)) {

      if (is_fpu_reg(regno)) {
        // Save S0
        write32(debug_abstract, i++, csrw(S0, CSR_DSCRATCH0));
        // Save mstatus
        write32(debug_abstract, i++, csrr(S0, CSR_MSTATUS));
        write32(debug_abstract, i++, csrw(S0, CSR_DSCRATCH1));
        // Set mstatus.fs
        assert((MSTATUS_FS & 0xfff) == 0);
        write32(debug_abstract, i++, lui(S0, MSTATUS_FS >> 12));
        write32(debug_abstract, i++, csrrs(ZERO, S0, CSR_MSTATUS));
      }

      if (regno < 0x1000 && config.support_abstract_csr_access) {
        if (!is_fpu_reg(regno)) {
          write32(debug_abstract, i++, csrw(S0, CSR_DSCRATCH0));
        }

        if (write) {
          switch (size) {
            case 2:
              write32(debug_abstract, i++, lw(S0, ZERO, debug_data_start));
              break;
            case 3:
              write32(debug_abstract, i++, ld(S0, ZERO, debug_data_start));
              break;
            default:
              abstractcs.cmderr = CMDERR_NOTSUP;
              return true;
          }
          write32(debug_abstract, i++, csrw(S0, regno));

        } else {
          write32(debug_abstract, i++, csrr(S0, regno));
          switch (size) {
            case 2:
              write32(debug_abstract, i++, sw(S0, ZERO, debug_data_start));
              break;
            case 3:
              write32(debug_abstract, i++, sd(S0, ZERO, debug_data_start));
              break;
            default:
              abstractcs.cmderr = CMDERR_NOTSUP;
              return true;
          }
        }
        if (!is_fpu_reg(regno)) {
          write32(debug_abstract, i++, csrr(S0, CSR_DSCRATCH0));
        }

      } else if (regno >= 0x1000 && regno < 0x1020) {
        unsigned regnum = regno - 0x1000;

        switch (size) {
          case 2:
            if (write)
              write32(debug_abstract, i++, lw(regnum, ZERO, debug_data_start));
            else
              write32(debug_abstract, i++, sw(regnum, ZERO, debug_data_start));
            break;
          case 3:
            if (write)
              write32(debug_abstract, i++, ld(regnum, ZERO, debug_data_start));
            else
              write32(debug_abstract, i++, sd(regnum, ZERO, debug_data_start));
            break;
          default:
            abstractcs.cmderr = CMDERR_NOTSUP;
            return true;
        }

        if (regno == 0x1000 + S0 && write) {
          /*
           * The exception handler starts out be restoring dscratch to s0,
           * which was saved before executing the abstract memory region. Since
           * we just wrote s0, also make sure to write that same value to
           * dscratch in case an exception occurs in a program buffer that
           * might be executed later.
           */
          write32(debug_abstract, i++, csrw(S0, CSR_DSCRATCH0));
        }

      } else if (regno >= 0x1020 && regno < 0x1040) {
        unsigned fprnum = regno - 0x1020;

        if (write) {
          switch (size) {
            case 2:
              write32(debug_abstract, i++, flw(fprnum, ZERO, debug_data_start));
              break;
            case 3:
              write32(debug_abstract, i++, fld(fprnum, ZERO, debug_data_start));
              break;
            default:
              abstractcs.cmderr = CMDERR_NOTSUP;
              return true;
          }

        } else {
          switch (size) {
            case 2:
              write32(debug_abstract, i++, fsw(fprnum, ZERO, debug_data_start));
              break;
            case 3:
              write32(debug_abstract, i++, fsd(fprnum, ZERO, debug_data_start));
              break;
            default:
              abstractcs.cmderr = CMDERR_NOTSUP;
              return true;
          }
        }

      } else if (regno >= 0xc000 && (regno & 1) == 1) {
        // Support odd-numbered custom registers, to allow for debugger testing.
        unsigned custom_number = regno - 0xc000;
        abstractcs.cmderr = CMDERR_NONE;
        if (write) {
          // Writing V to custom register N will cause future reads of N to
          // return V, reads of N-1 will return V-1, etc.
          custom_base = read32(dmdata, 0) - custom_number;
        } else {
          write32(dmdata, 0, custom_number + custom_base);
          write32(dmdata, 1, 0);
        }
        return true;

      } else {
        abstractcs.cmderr = CMDERR_NOTSUP;
        return true;
      }

      if (is_fpu_reg(regno)) {
        // restore mstatus
        write32(debug_abstract, i++, csrr(S0, CSR_DSCRATCH1));
        write32(debug_abstract, i++, csrw(S0, CSR_MSTATUS));
        // restore s0
        write32(debug_abstract, i++, csrr(S0, CSR_DSCRATCH0));
      }
    }

    if (get_field(command, AC_ACCESS_REGISTER_POSTEXEC)) {
      write32(debug_abstract, i,
          jal(ZERO, debug_progbuf_start - debug_abstract_start - 4 * i));
      i++;
    } else {
      write32(debug_abstract, i++, ebreak());
    }

    debug_rom_flags[dmcontrol.hartsel] |= 1 << DEBUG_ROM_FLAG_GO;
    rti_remaining = config.abstract_rti;
    abstract_command_completed = false;

    abstractcs.busy = true;
  } else {
    abstractcs.cmderr = CMDERR_NOTSUP;
  }
  return true;
}

bool debug_module_t::dmi_write(unsigned address, uint32_t value)
{
  D(fprintf(stderr, "dmi_write(0x%x, 0x%x)\n", address, value));

  if (!dmstatus.authenticated && address != DMI_AUTHDATA &&
      address != DMI_DMCONTROL)
    return false;

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

  } else if (address >= DMI_PROGBUF0 && address < DMI_PROGBUF0 + config.progbufsize) {
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
          if (!dmstatus.authenticated || !dmcontrol.dmactive)
            return true;

          dmcontrol.haltreq = get_field(value, DMI_DMCONTROL_HALTREQ);
          dmcontrol.resumereq = get_field(value, DMI_DMCONTROL_RESUMEREQ);
          dmcontrol.hartreset = get_field(value, DMI_DMCONTROL_HARTRESET);
          dmcontrol.ndmreset = get_field(value, DMI_DMCONTROL_NDMRESET);
          if (config.support_hasel)
            dmcontrol.hasel = get_field(value, DMI_DMCONTROL_HASEL);
          else
            dmcontrol.hasel = 0;
          dmcontrol.hartsel = get_field(value, DMI_DMCONTROL_HARTSELHI) <<
            DMI_DMCONTROL_HARTSELLO_LENGTH;
          dmcontrol.hartsel |= get_field(value, DMI_DMCONTROL_HARTSELLO);
          dmcontrol.hartsel &= (1L<<hartsellen) - 1;
          for (unsigned i = 0; i < nprocs; i++) {
            if (hart_selected(i)) {
              if (get_field(value, DMI_DMCONTROL_ACKHAVERESET)) {
                hart_state[i].havereset = false;
              }
              processor_t *proc = processor(i);
              if (proc) {
                proc->halt_request = dmcontrol.haltreq ? proc->HR_REGULAR : proc->HR_NONE;
                if (dmcontrol.haltreq) {
                  D(fprintf(stderr, "halt hart %d\n", i));
                }
                if (dmcontrol.resumereq) {
                  D(fprintf(stderr, "resume hart %d\n", i));
                  debug_rom_flags[i] |= (1 << DEBUG_ROM_FLAG_RESUME);
                  hart_state[i].resumeack = false;
                }
                if (dmcontrol.hartreset) {
                  proc->reset();
                }
              }
            }
          }

          if (dmcontrol.ndmreset) {
            for (size_t i = 0; i < sim->nprocs(); i++) {
              processor_t *proc = sim->get_core(i);
              proc->reset();
            }
          }
        }
        return true;

      case DMI_COMMAND:
        command = value;
        return perform_abstract_command();

      case DMI_HAWINDOWSEL:
        hawindowsel = value & ((1U<<field_width(nprocs))-1);
        return true;

      case DMI_HAWINDOW:
        {
          unsigned base = hawindowsel * 32;
          for (unsigned i = 0; i < 32; i++) {
            unsigned n = base + i;
            if (n < nprocs) {
              hart_array_mask[n] = (value >> i) & 1;
            }
          }
        }
        return true;

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
          sb_autoincrement();
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
          if (sbcs.error == 0) {
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
      case DMI_AUTHDATA:
        D(fprintf(stderr, "debug authentication: got 0x%x; 0x%x unlocks\n", value,
            challenge + secret));
        if (config.require_authentication) {
          if (value == challenge + secret) {
            dmstatus.authenticated = true;
          } else {
            dmstatus.authenticated = false;
            challenge = random();
          }
        }
        return true;
      case DMI_DMCS2:
        if (config.support_haltgroups && get_field(value, DMI_DMCS2_HGWRITE)) {
          hart_state[dmcontrol.hartsel].haltgroup = get_field(value,
              DMI_DMCS2_HALTGROUP);
        }
        return true;
    }
  }
  return false;
}

void debug_module_t::proc_reset(unsigned id)
{
  hart_state[id].havereset = true;
  hart_state[id].halted = false;
  hart_state[id].haltgroup = 0;
}
