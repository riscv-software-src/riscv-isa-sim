#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>

#include "disasm.h"
#include "sim.h"
#include "gdbserver.h"
#include "mmu.h"

#define C_EBREAK        0x9002
#define EBREAK          0x00100073

//////////////////////////////////////// Utility Functions

#undef DEBUG
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif // DEBUG

const int debug_gdbserver = 0;

void die(const char* msg)
{
  fprintf(stderr, "gdbserver code died: %s\n", msg);
  abort();
}

// gdb's register list is defined in riscv_gdb_reg_names gdb/riscv-tdep.c in
// its source tree. We must interpret the numbers the same here.
enum {
  REG_XPR0 = 0,
  REG_XPR31 = 31,
  REG_PC = 32,
  REG_FPR0 = 33,
  REG_FPR31 = 64,
  REG_CSR0 = 65,
  REG_CSR4095 = 4160,
  REG_END = 4161
};

//////////////////////////////////////// Functions to generate RISC-V opcodes.

// TODO: Does this already exist somewhere?

#define ZERO    0
// Using regnames.cc as source. The RVG Calling Convention of the 2.0 RISC-V
// spec says it should be 2 and 3.
#define S0      8
#define S1      9
static uint32_t bits(uint32_t value, unsigned int hi, unsigned int lo) {
  return (value >> lo) & ((1 << (hi+1-lo)) - 1);
}

static uint32_t bit(uint32_t value, unsigned int b) {
  return (value >> b) & 1;
}

static uint32_t jal(unsigned int rd, uint32_t imm) {
  return (bit(imm, 20) << 31) |
    (bits(imm, 10, 1) << 21) |
    (bit(imm, 11) << 20) |
    (bits(imm, 19, 12) << 12) |
    (rd << 7) |
    MATCH_JAL;
}

static uint32_t csrsi(unsigned int csr, uint16_t imm) {
  return (csr << 20) |
    (bits(imm, 4, 0) << 15) |
    MATCH_CSRRSI;
}

static uint32_t csrci(unsigned int csr, uint16_t imm) {
  return (csr << 20) |
    (bits(imm, 4, 0) << 15) |
    MATCH_CSRRCI;
}

static uint32_t csrr(unsigned int rd, unsigned int csr) {
  return (csr << 20) | (rd << 7) | MATCH_CSRRS;
}

static uint32_t csrw(unsigned int source, unsigned int csr) {
  return (csr << 20) | (source << 15) | MATCH_CSRRW;
}

static uint32_t fence_i()
{
  return MATCH_FENCE_I;
}

static uint32_t sb(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SB;
}

static uint32_t sh(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SH;
}

static uint32_t sw(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (src << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SW;
}

static uint32_t sd(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SD;
}

static uint32_t sq(unsigned int src, unsigned int base, uint16_t offset)
{
#if 0
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_SQ;
#else
  abort();
#endif
}

static uint32_t lq(unsigned int rd, unsigned int base, uint16_t offset)
{
#if 0
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LQ;
#else
  abort();
#endif
}

static uint32_t ld(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LD;
}

static uint32_t lw(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LW;
}

static uint32_t lh(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LH;
}

static uint32_t lb(unsigned int rd, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 0) << 20) |
    (base << 15) |
    (bits(rd, 4, 0) << 7) |
    MATCH_LB;
}

static uint32_t fsw(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FSW;
}

static uint32_t fsd(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FSD;
}

static uint32_t flw(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FLW;
}

static uint32_t fld(unsigned int src, unsigned int base, uint16_t offset)
{
  return (bits(offset, 11, 5) << 25) |
    (bits(src, 4, 0) << 20) |
    (base << 15) |
    (bits(offset, 4, 0) << 7) |
    MATCH_FLD;
}

static uint32_t addi(unsigned int dest, unsigned int src, uint16_t imm)
{
  return (bits(imm, 11, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_ADDI;
}

static uint32_t ori(unsigned int dest, unsigned int src, uint16_t imm)
{
  return (bits(imm, 11, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_ORI;
}

static uint32_t xori(unsigned int dest, unsigned int src, uint16_t imm)
{
  return (bits(imm, 11, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_XORI;
}

static uint32_t srli(unsigned int dest, unsigned int src, uint8_t shamt)
{
  return (bits(shamt, 4, 0) << 20) |
    (src << 15) |
    (dest << 7) |
    MATCH_SRLI;
}


static uint32_t nop()
{
  return addi(0, 0, 0);
}

template <typename T>
unsigned int circular_buffer_t<T>::size() const
{
  if (end >= start)
    return end - start;
  else
    return end + capacity - start;
}

template <typename T>
void circular_buffer_t<T>::consume(unsigned int bytes)
{
  start = (start + bytes) % capacity;
}

template <typename T>
unsigned int circular_buffer_t<T>::contiguous_empty_size() const
{
  if (end >= start)
    if (start == 0)
      return capacity - end - 1;
    else
      return capacity - end;
  else
    return start - end - 1;
}

template <typename T>
unsigned int circular_buffer_t<T>::contiguous_data_size() const
{
  if (end >= start)
    return end - start;
  else
    return capacity - start;
}

template <typename T>
void circular_buffer_t<T>::data_added(unsigned int bytes)
{
  end += bytes;
  assert(end <= capacity);
  if (end == capacity)
    end = 0;
}

template <typename T>
void circular_buffer_t<T>::reset()
{
  start = 0;
  end = 0;
}

template <typename T>
void circular_buffer_t<T>::append(const T *src, unsigned int count)
{
  unsigned int copy = std::min(count, contiguous_empty_size());
  memcpy(contiguous_empty(), src, copy * sizeof(T));
  data_added(copy);
  count -= copy;
  if (count > 0) {
    assert(count < contiguous_empty_size());
    memcpy(contiguous_empty(), src, count * sizeof(T));
    data_added(count);
  }
}

////////////////////////////// Debug Operations

class halt_op_t : public operation_t
{
  public:
    halt_op_t(gdbserver_t& gdbserver, bool send_status=false) :
      operation_t(gdbserver), send_status(send_status),
      state(ST_ENTER) {};

    void write_dpc_program() {
      gs.dr_write32(0, csrsi(CSR_DCSR, DCSR_HALT));
      gs.dr_write32(1, csrr(S0, CSR_DPC));
      gs.dr_write_store(2, S0, SLOT_DATA0);
      gs.dr_write_jump(3);
      gs.set_interrupt(0);
    }

    bool perform_step(unsigned int step) {
      switch (state) {
        case ST_ENTER:
          if (gs.xlen == 0) {
            gs.dr_write32(0, xori(S1, ZERO, -1));
            gs.dr_write32(1, srli(S1, S1, 31));
            // 0x00000001  0x00000001:ffffffff  0x00000001:ffffffff:ffffffff:ffffffff
            gs.dr_write32(2, sw(S1, ZERO, DEBUG_RAM_START));
            gs.dr_write32(3, srli(S1, S1, 31));
            // 0x00000000  0x00000000:00000003  0x00000000:00000003:ffffffff:ffffffff
            gs.dr_write32(4, sw(S1, ZERO, DEBUG_RAM_START + 4));
            gs.dr_write_jump(5);
            gs.set_interrupt(0);
            state = ST_XLEN;

          } else {
            write_dpc_program();
            state = ST_DPC;
          }
          return false;

        case ST_XLEN:
          {
            uint32_t word0 = gs.dr_read32(0);
            uint32_t word1 = gs.dr_read32(1);

            if (word0 == 1 && word1 == 0) {
              gs.xlen = 32;
            } else if (word0 == 0xffffffff && word1 == 3) {
              gs.xlen = 64;
            } else if (word0 == 0xffffffff && word1 == 0xffffffff) {
              gs.xlen = 128;
            }

            write_dpc_program();
            state = ST_DPC;
            return false;
          }

        case ST_DPC:
          gs.dpc = gs.dr_read(SLOT_DATA0);
          fprintf(stderr, "dpc=0x%lx\n", gs.dpc);
          gs.dr_write32(0, csrr(S0, CSR_MSTATUS));
          gs.dr_write_store(1, S0, SLOT_DATA0);
          gs.dr_write_jump(2);
          gs.set_interrupt(0);
          state = ST_MSTATUS;
          return false;

        case ST_MSTATUS:
          gs.mstatus = gs.dr_read(SLOT_DATA0);
          gs.dr_write32(0, csrr(S0, CSR_DCSR));
          gs.dr_write32(1, sw(S0, 0, (uint16_t) DEBUG_RAM_START + 16));
          gs.dr_write_jump(2);
          gs.set_interrupt(0);
          state = ST_DCSR;
          return false;

        case ST_DCSR:
          gs.dcsr = gs.dr_read32(4);

          gs.sptbr_valid = false;
          gs.pte_cache.clear();

          if (send_status) {
            switch (get_field(gs.dcsr, DCSR_CAUSE)) {
              case DCSR_CAUSE_NONE:
                fprintf(stderr, "Internal error. Processor halted without reason.\n");
                abort();

              case DCSR_CAUSE_DEBUGINT:
                gs.send_packet("S02");   // Pretend program received SIGINT.
                break;

              case DCSR_CAUSE_HWBP:
              case DCSR_CAUSE_STEP:
              case DCSR_CAUSE_HALT:
                // There's no gdb code for this.
                gs.send_packet("T05");
                break;
              case DCSR_CAUSE_SWBP:
                gs.send_packet("T05swbreak:;");
                break;
            }
          }
          return true;

        default:
          assert(0);
      }
    }

  private:
    bool send_status;
    enum {
      ST_ENTER,
      ST_XLEN,
      ST_DPC,
      ST_MSTATUS,
      ST_DCSR
    } state;
};

class continue_op_t : public operation_t
{
  public:
    continue_op_t(gdbserver_t& gdbserver, bool single_step) :
      operation_t(gdbserver), single_step(single_step) {};

    bool perform_step(unsigned int step) {
      switch (step) {
        case 0:
          gs.dr_write_load(0, S0, SLOT_DATA0);
          gs.dr_write32(1, csrw(S0, CSR_DPC));
          // TODO: Isn't there a fence.i in Debug ROM already?
          if (gs.fence_i_required) {
            gs.dr_write32(2, fence_i());
            gs.dr_write_jump(3);
            gs.fence_i_required = false;
          } else {
            gs.dr_write_jump(2);
          }
          gs.dr_write(SLOT_DATA0, gs.dpc);
          gs.set_interrupt(0);
          return false;

        case 1:
          gs.dr_write_load(0, S0, SLOT_DATA0);
          gs.dr_write32(1, csrw(S0, CSR_MSTATUS));
          gs.dr_write_jump(2);
          gs.dr_write(SLOT_DATA0, gs.mstatus);
          gs.set_interrupt(0);
          return false;

        case 2:
          gs.dr_write32(0, lw(S0, 0, (uint16_t) DEBUG_RAM_START+16));
          gs.dr_write32(1, csrw(S0, CSR_DCSR));
          gs.dr_write_jump(2);

          reg_t dcsr = set_field(gs.dcsr, DCSR_HALT, 0);
          dcsr = set_field(dcsr, DCSR_STEP, single_step);
          // Software breakpoints should go here.
          dcsr = set_field(dcsr, DCSR_EBREAKM, 1);
          dcsr = set_field(dcsr, DCSR_EBREAKH, 1);
          dcsr = set_field(dcsr, DCSR_EBREAKS, 1);
          dcsr = set_field(dcsr, DCSR_EBREAKU, 1);
          gs.dr_write32(4, dcsr);

          gs.set_interrupt(0);
          return true;
      }
      return false;
    }

  private:
    bool single_step;
};

class general_registers_read_op_t : public operation_t
{
  // Register order that gdb expects is:
  //   "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",
  //   "x8",  "x9",  "x10", "x11", "x12", "x13", "x14", "x15",
  //   "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
  //   "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31",

  // Each byte of register data is described by two hex digits. The bytes with
  // the register are transmitted in target byte order. The size of each
  // register and their position within the ‘g’ packet are determined by the
  // gdb internal gdbarch functions DEPRECATED_REGISTER_RAW_SIZE and
  // gdbarch_register_name.

  public:
    general_registers_read_op_t(gdbserver_t& gdbserver) :
      operation_t(gdbserver) {};

    bool perform_step(unsigned int step)
    {
      if (step == 0) {
        gs.start_packet();

        // x0 is always zero.
        if (gs.xlen == 32) {
          gs.send((uint32_t) 0);
        } else {
          gs.send((uint64_t) 0);
        }

        gs.dr_write_store(0, 1, SLOT_DATA0);
        gs.dr_write_store(1, 2, SLOT_DATA1);
        gs.dr_write_jump(2);
        gs.set_interrupt(0);
        return false;
      }

      if (gs.xlen == 32) {
        gs.send((uint32_t) gs.dr_read(SLOT_DATA0));
      } else {
        gs.send((uint64_t) gs.dr_read(SLOT_DATA0));
      }
      if (step >= 16) {
        gs.end_packet();
        return true;
      }

      if (gs.xlen == 32) {
        gs.send((uint32_t) gs.dr_read(SLOT_DATA1));
      } else {
        gs.send((uint64_t) gs.dr_read(SLOT_DATA1));
      }

      unsigned int current_reg = 2 * step + 1;
      unsigned int i = 0;
      if (current_reg == S1) {
        gs.dr_write_load(i++, S1, SLOT_DATA_LAST);
      }
      gs.dr_write_store(i++, current_reg, SLOT_DATA0);
      if (current_reg + 1 == S0) {
        gs.dr_write32(i++, csrr(S0, CSR_DSCRATCH));
      }
      gs.dr_write_store(i++, current_reg+1, SLOT_DATA1);
      gs.dr_write_jump(i);
      gs.set_interrupt(0);

      return false;
    }
};

class register_read_op_t : public operation_t
{
  public:
    register_read_op_t(gdbserver_t& gdbserver, unsigned int reg) :
      operation_t(gdbserver), reg(reg) {};

    bool perform_step(unsigned int step)
    {
      switch (step) {
        case 0:
          if (reg >= REG_XPR0 && reg <= REG_XPR31) {
            die("handle_register_read");
            // send(p->state.XPR[reg - REG_XPR0]);
          } else if (reg == REG_PC) {
            gs.start_packet();
            if (gs.xlen == 32) {
              gs.send((uint32_t) gs.dpc);
            } else {
              gs.send(gs.dpc);
            }
            gs.end_packet();
            return true;
          } else if (reg >= REG_FPR0 && reg <= REG_FPR31) {
            // send(p->state.FPR[reg - REG_FPR0]);
            if (gs.xlen == 32) {
              gs.dr_write32(0, fsw(reg - REG_FPR0, 0, (uint16_t) DEBUG_RAM_START + 16));
            } else {
              gs.dr_write32(0, fsd(reg - REG_FPR0, 0, (uint16_t) DEBUG_RAM_START + 16));
            }
            gs.dr_write_jump(1);
          } else if (reg >= REG_CSR0 && reg <= REG_CSR4095) {
            gs.dr_write32(0, csrr(S0, reg - REG_CSR0));
            gs.dr_write_store(1, S0, SLOT_DATA0);
            gs.dr_write_jump(2);
            // If we hit an exception reading the CSR, we'll end up returning ~0 as
            // the register's value, which is what we want. (Right?)
            gs.dr_write(SLOT_DATA0, ~(uint64_t) 0);
          } else {
            gs.send_packet("E02");
            return true;
          }
          gs.set_interrupt(0);
          return false;

        case 1:
          gs.start_packet();
          if (gs.xlen == 32) {
            gs.send(gs.dr_read32(4));
          } else {
            gs.send(gs.dr_read(SLOT_DATA0));
          }
          gs.end_packet();
          return true;
      }
      return false;
    }

  private:
    unsigned int reg;
};

class register_write_op_t : public operation_t
{
  public:
    register_write_op_t(gdbserver_t& gdbserver, unsigned int reg, reg_t value) :
      operation_t(gdbserver), reg(reg), value(value) {};

    bool perform_step(unsigned int step)
    {
      gs.dr_write_load(0, S0, SLOT_DATA0);
      gs.dr_write(SLOT_DATA0, value);
      if (reg == S0) {
        gs.dr_write32(1, csrw(S0, CSR_DSCRATCH));
        gs.dr_write_jump(2);
      } else if (reg == S1) {
        gs.dr_write_store(1, S0, SLOT_DATA_LAST);
        gs.dr_write_jump(2);
      } else if (reg >= REG_XPR0 && reg <= REG_XPR31) {
        gs.dr_write32(1, addi(reg, S0, 0));
        gs.dr_write_jump(2);
      } else if (reg == REG_PC) {
        gs.dpc = value;
        return true;
      } else if (reg >= REG_FPR0 && reg <= REG_FPR31) {
        if (gs.xlen == 32) {
          gs.dr_write32(0, flw(reg - REG_FPR0, 0, (uint16_t) DEBUG_RAM_START + 16));
        } else {
          gs.dr_write32(0, fld(reg - REG_FPR0, 0, (uint16_t) DEBUG_RAM_START + 16));
        }
        gs.dr_write_jump(1);
      } else if (reg >= REG_CSR0 && reg <= REG_CSR4095) {
        gs.dr_write32(1, csrw(S0, reg - REG_CSR0));
        gs.dr_write_jump(2);
        if (reg == REG_CSR0 + CSR_SPTBR) {
          gs.sptbr = value;
          gs.sptbr_valid = true;
        }
      } else {
        gs.send_packet("E02");
        return true;
      }
      gs.set_interrupt(0);
      gs.send_packet("OK");
      return true;
    }

  private:
    unsigned int reg;
    reg_t value;
};

class memory_read_op_t : public operation_t
{
  public:
    // Read length bytes from vaddr, storing the result into data.
    // If data is NULL, send the result straight to gdb.
    memory_read_op_t(gdbserver_t& gdbserver, reg_t vaddr, unsigned int length,
        unsigned char *data=NULL) :
      operation_t(gdbserver), vaddr(vaddr), length(length), data(data) {};

    bool perform_step(unsigned int step)
    {
      if (step == 0) {
        // address goes in S0
        paddr = gs.translate(vaddr);
        access_size = gs.find_access_size(paddr, length);

        gs.dr_write_load(0, S0, SLOT_DATA0);
        switch (access_size) {
          case 1:
            gs.dr_write32(1, lb(S1, S0, 0));
            break;
          case 2:
            gs.dr_write32(1, lh(S1, S0, 0));
            break;
          case 4:
            gs.dr_write32(1, lw(S1, S0, 0));
            break;
          case 8:
            gs.dr_write32(1, ld(S1, S0, 0));
            break;
        }
        gs.dr_write_store(2, S1, SLOT_DATA1);
        gs.dr_write_jump(3);
        gs.dr_write(SLOT_DATA0, paddr);
        gs.set_interrupt(0);

        if (!data) {
          gs.start_packet();
        }
        return false;
      }

      char buffer[3];
      reg_t value = gs.dr_read(SLOT_DATA1);
      for (unsigned int i = 0; i < access_size; i++) {
        if (data) {
          *(data++) = value & 0xff;
          D(fprintf(stderr, "%02x", (unsigned int) (value & 0xff)));
        } else {
          sprintf(buffer, "%02x", (unsigned int) (value & 0xff));
          gs.send(buffer);
        }
        value >>= 8;
      }
      if (data && debug_gdbserver) {
        D(fprintf(stderr, "\n"));
      }
      length -= access_size;
      paddr += access_size;

      if (length == 0) {
        if (!data) {
          gs.end_packet();
        }
        return true;
      } else {
        gs.dr_write(SLOT_DATA0, paddr);
        gs.set_interrupt(0);
        return false;
      }
    }

  private:
    reg_t vaddr;
    unsigned int length;
    unsigned char* data;
    reg_t paddr;
    unsigned int access_size;
};

class memory_write_op_t : public operation_t
{
  public:
    memory_write_op_t(gdbserver_t& gdbserver, reg_t vaddr, unsigned int length,
        const unsigned char *data) :
      operation_t(gdbserver), vaddr(vaddr), offset(0), length(length), data(data) {};

    ~memory_write_op_t() {
      delete[] data;
    }

    bool perform_step(unsigned int step)
    {
      reg_t paddr = gs.translate(vaddr);
      if (step == 0) {
        access_size = gs.find_access_size(paddr, length);

        D(fprintf(stderr, "write to 0x%lx -> 0x%lx (access=%d): ", vaddr, paddr,
            access_size));
        for (unsigned int i = 0; i < length; i++) {
          D(fprintf(stderr, "%02x", data[i]));
        }
        D(fprintf(stderr, "\n"));

        // address goes in S0
        gs.dr_write_load(0, S0, SLOT_DATA0);
        switch (access_size) {
          case 1:
            gs.dr_write32(1, lb(S1, 0, (uint16_t) DEBUG_RAM_START + 24));
            gs.dr_write32(2, sb(S1, S0, 0));
            gs.dr_write32(6, data[0]);
            break;
          case 2:
            gs.dr_write32(1, lh(S1, 0, (uint16_t) DEBUG_RAM_START + 24));
            gs.dr_write32(2, sh(S1, S0, 0));
            gs.dr_write32(6, data[0] | (data[1] << 8));
            break;
          case 4:
            gs.dr_write32(1, lw(S1, 0, (uint16_t) DEBUG_RAM_START + 24));
            gs.dr_write32(2, sw(S1, S0, 0));
            gs.dr_write32(6, data[0] | (data[1] << 8) |
                (data[2] << 16) | (data[3] << 24));
            break;
          case 8:
            gs.dr_write32(1, ld(S1, 0, (uint16_t) DEBUG_RAM_START + 24));
            gs.dr_write32(2, sd(S1, S0, 0));
            gs.dr_write32(6, data[0] | (data[1] << 8) |
                (data[2] << 16) | (data[3] << 24));
            gs.dr_write32(7, data[4] | (data[5] << 8) |
                (data[6] << 16) | (data[7] << 24));
            break;
          default:
            fprintf(stderr, "gdbserver error: write %d bytes to 0x%lx -> 0x%lx; "
                "access_size=%d\n", length, vaddr, paddr, access_size);
            gs.send_packet("E12");
            return true;
        }
        gs.dr_write_jump(3);
        gs.dr_write(SLOT_DATA0, paddr);
        gs.set_interrupt(0);

        return false;
      }

      if (gs.dr_read32(DEBUG_RAM_SIZE / 4 - 1)) {
        fprintf(stderr, "Exception happened while writing to 0x%lx -> 0x%lx\n",
            vaddr, paddr);
      }

      offset += access_size;
      if (offset >= length) {
        gs.send_packet("OK");
        return true;
      } else {
        const unsigned char *d = data + offset;
        switch (access_size) {
          case 1:
            gs.dr_write32(6, d[0]);
            break;
          case 2:
            gs.dr_write32(6, d[0] | (d[1] << 8));
            break;
          case 4:
            gs.dr_write32(6, d[0] | (d[1] << 8) |
                (d[2] << 16) | (d[3] << 24));
            break;
          case 8:
            gs.dr_write32(6, d[0] | (d[1] << 8) |
                (d[2] << 16) | (d[3] << 24));
            gs.dr_write32(7, d[4] | (d[5] << 8) |
                (d[6] << 16) | (d[7] << 24));
            break;
          default:
            gs.send_packet("E13");
            return true;
        }
        gs.dr_write(SLOT_DATA0, paddr + offset);
        gs.set_interrupt(0);
        return false;
      }
    }

  private:
    reg_t vaddr;
    unsigned int offset;
    unsigned int length;
    unsigned int access_size;
    const unsigned char *data;
};

class collect_translation_info_op_t : public operation_t
{
  public:
    // Read sufficient information from the target into gdbserver structures so
    // that it's possible to translate vaddr, vaddr+length, and all addresses
    // in between to physical addresses.
    collect_translation_info_op_t(gdbserver_t& gdbserver, reg_t vaddr, size_t length) :
      operation_t(gdbserver), state(STATE_START), vaddr(vaddr), length(length) {};

    bool perform_step(unsigned int step)
    {
      unsigned int vm = gs.virtual_memory();

      if (step == 0) {
        switch (vm) {
          case VM_MBARE:
            // Nothing to be done.
            return true;

          case VM_SV32:
            levels = 2;
            ptidxbits = 10;
            ptesize = 4;
            break;
          case VM_SV39:
            levels = 3;
            ptidxbits = 9;
            ptesize = 8;
            break;
          case VM_SV48:
            levels = 4;
            ptidxbits = 9;
            ptesize = 8;
            break;

          default:
            {
              char buf[100];
              sprintf(buf, "VM mode %d is not supported by gdbserver.cc.", vm);
              die(buf);
              return true;        // die doesn't return, but gcc doesn't know that.
            }
        }
      }

      // Perform any reads from the just-completed action.
      switch (state) {
        case STATE_START:
          break;
        case STATE_READ_SPTBR:
          gs.sptbr = ((uint64_t) gs.dr_read32(5) << 32) | gs.dr_read32(4);
          gs.sptbr_valid = true;
          break;
        case STATE_READ_PTE:
          gs.pte_cache[pte_addr] = ((uint64_t) gs.dr_read32(5) << 32) |
            gs.dr_read32(4);
          D(fprintf(stderr, "pte_cache[0x%lx] = 0x%lx\n", pte_addr, gs.pte_cache[pte_addr]));
          break;
      }

      // Set up the next action.
      // We only get here for VM_SV32/39/38.

      if (!gs.sptbr_valid) {
        state = STATE_READ_SPTBR;
        gs.dr_write32(0, csrr(S0, CSR_SPTBR));
        gs.dr_write32(1, sd(S0, 0, (uint16_t) DEBUG_RAM_START + 16));
        gs.dr_write32(2, jal(0, (uint32_t) (DEBUG_ROM_RESUME - (DEBUG_RAM_START + 4*2))));
        gs.set_interrupt(0);
        return false;
      }

      reg_t base = gs.sptbr << PGSHIFT;
      int ptshift = (levels - 1) * ptidxbits;
      for (unsigned int i = 0; i < levels; i++, ptshift -= ptidxbits) {
        reg_t idx = (vaddr >> (PGSHIFT + ptshift)) & ((1 << ptidxbits) - 1);

        pte_addr = base + idx * ptesize;
        auto it = gs.pte_cache.find(pte_addr);
        if (it == gs.pte_cache.end()) {
          state = STATE_READ_PTE;
          if (ptesize == 4) {
            gs.dr_write32(0, lw(S0, 0, (uint16_t) DEBUG_RAM_START + 16));
            gs.dr_write32(1, lw(S1, S0, 0));
            gs.dr_write32(2, sw(S1, 0, (uint16_t) DEBUG_RAM_START + 16));
          } else {
            assert(gs.xlen >= 64);
            gs.dr_write32(0, ld(S0, 0, (uint16_t) DEBUG_RAM_START + 16));
            gs.dr_write32(1, ld(S1, S0, 0));
            gs.dr_write32(2, sd(S1, 0, (uint16_t) DEBUG_RAM_START + 16));
          }
          gs.dr_write32(3, jal(0, (uint32_t) (DEBUG_ROM_RESUME - (DEBUG_RAM_START + 4*3))));
          gs.dr_write32(4, pte_addr);
          gs.dr_write32(5, pte_addr >> 32);
          gs.set_interrupt(0);
          return false;
        }

        reg_t pte = gs.pte_cache[pte_addr];
        reg_t ppn = pte >> PTE_PPN_SHIFT;

        if (PTE_TABLE(pte)) { // next level of page table
          base = ppn << PGSHIFT;
        } else {
          // We've collected all the data required for the translation.
          return true;
        }
      }
      fprintf(stderr,
          "ERROR: gdbserver couldn't find appropriate PTEs to translate 0x%lx\n",
          vaddr);
      return true;
    }

  private:
    enum {
      STATE_START,
      STATE_READ_SPTBR,
      STATE_READ_PTE
    } state;
    reg_t vaddr;
    size_t length;
    unsigned int levels;
    unsigned int ptidxbits;
    unsigned int ptesize;
    reg_t pte_addr;
};

////////////////////////////// gdbserver itself

gdbserver_t::gdbserver_t(uint16_t port, sim_t *sim) :
  xlen(0),
  sim(sim),
  client_fd(0),
  recv_buf(64 * 1024), send_buf(64 * 1024)
{
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "failed to make socket: %s (%d)\n", strerror(errno), errno);
    abort();
  }

  fcntl(socket_fd, F_SETFL, O_NONBLOCK);
  int reuseaddr = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
        sizeof(int)) == -1) {
    fprintf(stderr, "failed setsockopt: %s (%d)\n", strerror(errno), errno);
    abort();
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    fprintf(stderr, "failed to bind socket: %s (%d)\n", strerror(errno), errno);
    abort();
  }

  if (listen(socket_fd, 1) == -1) {
    fprintf(stderr, "failed to listen on socket: %s (%d)\n", strerror(errno), errno);
    abort();
  }
}

unsigned int gdbserver_t::find_access_size(reg_t address, int length)
{
  reg_t composite = address | length;
  if ((composite & 0x7) == 0 && xlen >= 64)
    return 8;
  if ((composite & 0x3) == 0)
    return 4;
  return 1;
}

reg_t gdbserver_t::translate(reg_t vaddr)
{
  unsigned int vm = virtual_memory();
  unsigned int levels, ptidxbits, ptesize;

  switch (vm) {
    case VM_MBARE:
      return vaddr;

    case VM_SV32:
      levels = 2;
      ptidxbits = 10;
      ptesize = 4;
      break;
    case VM_SV39:
      levels = 3;
      ptidxbits = 9;
      ptesize = 8;
      break;
    case VM_SV48:
      levels = 4;
      ptidxbits = 9;
      ptesize = 8;
      break;

    default:
      {
        char buf[100];
        sprintf(buf, "VM mode %d is not supported by gdbserver.cc.", vm);
        die(buf);
        return true;        // die doesn't return, but gcc doesn't know that.
      }
  }

  // Handle page tables here. There's a bunch of duplicated code with
  // collect_translation_info_op_t. :-(
  reg_t base = sptbr << PGSHIFT;
  int ptshift = (levels - 1) * ptidxbits;
  for (unsigned int i = 0; i < levels; i++, ptshift -= ptidxbits) {
    reg_t idx = (vaddr >> (PGSHIFT + ptshift)) & ((1 << ptidxbits) - 1);

    reg_t pte_addr = base + idx * ptesize;
    auto it = pte_cache.find(pte_addr);
    if (it == pte_cache.end()) {
      fprintf(stderr, "ERROR: gdbserver tried to translate 0x%lx without first "
          "collecting the relevant PTEs.\n", vaddr);
      die("gdbserver_t::translate()");
    }

    reg_t pte = pte_cache[pte_addr];
    reg_t ppn = pte >> PTE_PPN_SHIFT;

    if (PTE_TABLE(pte)) { // next level of page table
      base = ppn << PGSHIFT;
    } else {
      // We've collected all the data required for the translation.
      reg_t vpn = vaddr >> PGSHIFT;
      reg_t paddr = (ppn | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;
      paddr += vaddr & (PGSIZE-1);
      D(fprintf(stderr, "gdbserver translate 0x%lx -> 0x%lx\n", vaddr, paddr));
      return paddr;
    }
  }

  fprintf(stderr, "ERROR: gdbserver tried to translate 0x%lx but the relevant "
      "PTEs are invalid.\n", vaddr);
  // TODO: Is it better to throw an exception here?
  return -1;
}

unsigned int gdbserver_t::privilege_mode()
{
  unsigned int mode = get_field(dcsr, DCSR_PRV);
  if (get_field(mstatus, MSTATUS_MPRV))
    mode = get_field(mstatus, MSTATUS_MPP);
  return mode;
}

unsigned int gdbserver_t::virtual_memory()
{
  unsigned int mode = privilege_mode();
  if (mode == PRV_M)
    return VM_MBARE;
  return get_field(mstatus, MSTATUS_VM);
}

void gdbserver_t::dr_write32(unsigned int index, uint32_t value)
{
  sim->debug_module.ram_write32(index, value);
}

void gdbserver_t::dr_write64(unsigned int index, uint64_t value)
{
  dr_write32(index, value);
  dr_write32(index+1, value >> 32);
}

void gdbserver_t::dr_write(enum slot slot, uint64_t value)
{
  switch (xlen) {
    case 32:
      dr_write32(slot_offset32[slot], value);
      break;
    case 64:
      dr_write64(slot_offset64[slot], value);
      break;
    case 128:
    default:
      abort();
  }
}

void gdbserver_t::dr_write_jump(unsigned int index)
{
  dr_write32(index, jal(0,
        (uint32_t) (DEBUG_ROM_RESUME - (DEBUG_RAM_START + 4*index))));
}

void gdbserver_t::dr_write_store(unsigned int index, unsigned int reg, enum slot slot)
{
  assert(slot != SLOT_INST0 || index > 2);
  assert(slot != SLOT_DATA0 || index < 4 || index > 6);
  assert(slot != SLOT_DATA1 || index < 5 || index > 10);
  assert(slot != SLOT_DATA_LAST || index < 6 || index > 14);
  switch (xlen) {
    case 32:
      return dr_write32(index,
          sw(reg, 0, (uint16_t) DEBUG_RAM_START + 4 * slot_offset32[slot]));
    case 64:
      return dr_write32(index,
          sd(reg, 0, (uint16_t) DEBUG_RAM_START + 4 * slot_offset64[slot]));
    case 128:
      return dr_write32(index,
          sq(reg, 0, (uint16_t) DEBUG_RAM_START + 4 * slot_offset128[slot]));
    default:
      fprintf(stderr, "xlen is %d!\n", xlen);
      abort();
  }
}

void gdbserver_t::dr_write_load(unsigned int index, unsigned int reg, enum slot slot)
{
  switch (xlen) {
    case 32:
      return dr_write32(index,
          lw(reg, 0, (uint16_t) DEBUG_RAM_START + 4 * slot_offset32[slot]));
    case 64:
      return dr_write32(index,
          ld(reg, 0, (uint16_t) DEBUG_RAM_START + 4 * slot_offset64[slot]));
    case 128:
      return dr_write32(index,
          lq(reg, 0, (uint16_t) DEBUG_RAM_START + 4 * slot_offset128[slot]));
    default:
      fprintf(stderr, "xlen is %d!\n", xlen);
      abort();
  }
}

uint32_t gdbserver_t::dr_read32(unsigned int index)
{
  uint32_t value = sim->debug_module.ram_read32(index);
  D(fprintf(stderr, "read32(%d) -> 0x%x\n", index, value));
  return value;
}

uint64_t gdbserver_t::dr_read64(unsigned int index)
{
  return ((uint64_t) dr_read32(index+1) << 32) | dr_read32(index);
}

uint64_t gdbserver_t::dr_read(enum slot slot)
{
  switch (xlen) {
    case 32:
      return dr_read32(slot_offset32[slot]);
    case 64:
      return dr_read64(slot_offset64[slot]);
    case 128:
      abort();
    default:
      abort();
  }
}

void gdbserver_t::add_operation(operation_t* operation)
{
  operation_queue.push(operation);
}

void gdbserver_t::accept()
{
  client_fd = ::accept(socket_fd, NULL, NULL);
  if (client_fd == -1) {
    if (errno == EAGAIN) {
      // No client waiting to connect right now.
    } else {
      fprintf(stderr, "failed to accept on socket: %s (%d)\n", strerror(errno),
          errno);
      abort();
    }
  } else {
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    expect_ack = false;
    extended_mode = false;

    // gdb wants the core to be halted when it attaches.
    add_operation(new halt_op_t(*this));
  }
}

void gdbserver_t::read()
{
  // Reading from a non-blocking socket still blocks if there is no data
  // available.

  size_t count = recv_buf.contiguous_empty_size();
  assert(count > 0);
  ssize_t bytes = ::read(client_fd, recv_buf.contiguous_empty(), count);
  if (bytes == -1) {
    if (errno == EAGAIN) {
      // We'll try again the next call.
    } else {
      fprintf(stderr, "failed to read on socket: %s (%d)\n", strerror(errno), errno);
      abort();
    }
  } else if (bytes == 0) {
    // The remote disconnected.
    client_fd = 0;
    processor_t *p = sim->get_core(0);
    // TODO p->set_halted(false, HR_NONE);
    recv_buf.reset();
    send_buf.reset();
  } else {
    recv_buf.data_added(bytes);
  }
}

void gdbserver_t::write()
{
  if (send_buf.empty())
    return;

  while (!send_buf.empty()) {
    unsigned int count = send_buf.contiguous_data_size();
    assert(count > 0);
    ssize_t bytes = ::write(client_fd, send_buf.contiguous_data(), count);
    if (bytes == -1) {
      fprintf(stderr, "failed to write to socket: %s (%d)\n", strerror(errno), errno);
      abort();
    } else if (bytes == 0) {
      // Client can't take any more data right now.
      break;
    } else {
      D(fprintf(stderr, "wrote %ld bytes: ", bytes));
      for (unsigned int i = 0; i < bytes; i++) {
        D(fprintf(stderr, "%c", send_buf[i]));
      }
      D(fprintf(stderr, "\n"));
      send_buf.consume(bytes);
    }
  }
}

void print_packet(const std::vector<uint8_t> &packet)
{
  for (uint8_t c : packet) {
    if (c >= ' ' and c <= '~')
      fprintf(stderr, "%c", c);
    else
      fprintf(stderr, "\\x%02x", c);
  }
  fprintf(stderr, "\n");
}

uint8_t compute_checksum(const std::vector<uint8_t> &packet)
{
  uint8_t checksum = 0;
  for (auto i = packet.begin() + 1; i != packet.end() - 3; i++ ) {
    checksum += *i;
  }
  return checksum;
}

uint8_t character_hex_value(uint8_t character)
{
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'a' && character <= 'f')
    return 10 + character - 'a';
  if (character >= 'A' && character <= 'F')
    return 10 + character - 'A';
  return 0xff;
}

uint8_t extract_checksum(const std::vector<uint8_t> &packet)
{
  return character_hex_value(*(packet.end() - 1)) +
    16 * character_hex_value(*(packet.end() - 2));
}

void gdbserver_t::process_requests()
{
  // See https://sourceware.org/gdb/onlinedocs/gdb/Remote-Protocol.html

  while (!recv_buf.empty()) {
    std::vector<uint8_t> packet;
    for (unsigned int i = 0; i < recv_buf.size(); i++) {
      uint8_t b = recv_buf[i];

      if (packet.empty() && expect_ack && b == '+') {
        recv_buf.consume(1);
        break;
      }

      if (packet.empty() && b == 3) {
        D(fprintf(stderr, "Received interrupt\n"));
        recv_buf.consume(1);
        handle_interrupt();
        break;
      }

      if (b == '$') {
        // Start of new packet.
        if (!packet.empty()) {
          fprintf(stderr, "Received malformed %ld-byte packet from debug client: ",
              packet.size());
          print_packet(packet);
          recv_buf.consume(i);
          break;
        }
      }

      packet.push_back(b);

      // Packets consist of $<packet-data>#<checksum>
      // where <checksum> is 
      if (packet.size() >= 4 &&
          packet[packet.size()-3] == '#') {
        handle_packet(packet);
        recv_buf.consume(i+1);
        break;
      }
    }
    // There's a partial packet in the buffer. Wait until we get more data to
    // process it.
    if (packet.size()) {
      break;
    }
  }
}

void gdbserver_t::handle_halt_reason(const std::vector<uint8_t> &packet)
{
  send_packet("S00");
}

void gdbserver_t::handle_general_registers_read(const std::vector<uint8_t> &packet)
{
  add_operation(new general_registers_read_op_t(*this));
}

void gdbserver_t::set_interrupt(uint32_t hartid) {
  sim->debug_module.set_interrupt(hartid);
}

// First byte is the most-significant one.
// Eg. "08675309" becomes 0x08675309.
uint64_t consume_hex_number(std::vector<uint8_t>::const_iterator &iter,
    std::vector<uint8_t>::const_iterator end)
{
  uint64_t value = 0;

  while (iter != end) {
    uint8_t c = *iter;
    uint64_t c_value = character_hex_value(c);
    if (c_value > 15)
      break;
    iter++;
    value <<= 4;
    value += c_value;
  }
  return value;
}

// First byte is the least-significant one.
// Eg. "08675309" becomes 0x09536708
uint64_t consume_hex_number_le(std::vector<uint8_t>::const_iterator &iter,
    std::vector<uint8_t>::const_iterator end)
{
  uint64_t value = 0;
  unsigned int shift = 4;

  while (iter != end) {
    uint8_t c = *iter;
    uint64_t c_value = character_hex_value(c);
    if (c_value > 15)
      break;
    iter++;
    value |= c_value << shift;
    if ((shift % 8) == 0)
      shift += 12;
    else
      shift -= 4;
  }
  return value;
}

void consume_string(std::string &str, std::vector<uint8_t>::const_iterator &iter,
    std::vector<uint8_t>::const_iterator end, uint8_t separator)
{
  while (iter != end && *iter != separator) {
    str.append(1, (char) *iter);
    iter++;
  }
}

void gdbserver_t::handle_register_read(const std::vector<uint8_t> &packet)
{
  // p n

  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  unsigned int n = consume_hex_number(iter, packet.end());
  if (*iter != '#')
    return send_packet("E01");

  add_operation(new register_read_op_t(*this, n));
}

void gdbserver_t::handle_register_write(const std::vector<uint8_t> &packet)
{
  // P n...=r...

  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  unsigned int n = consume_hex_number(iter, packet.end());
  if (*iter != '=')
    return send_packet("E05");
  iter++;

  reg_t value = consume_hex_number_le(iter, packet.end());
  if (*iter != '#')
    return send_packet("E06");

  processor_t *p = sim->get_core(0);

  add_operation(new register_write_op_t(*this, n, value));

  return send_packet("OK");
}

void gdbserver_t::handle_memory_read(const std::vector<uint8_t> &packet)
{
  // m addr,length
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  reg_t address = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E10");
  iter++;
  reg_t length = consume_hex_number(iter, packet.end());
  if (*iter != '#')
    return send_packet("E11");

  add_operation(new collect_translation_info_op_t(*this, address, length));
  add_operation(new memory_read_op_t(*this, address, length));
}

void gdbserver_t::handle_memory_binary_write(const std::vector<uint8_t> &packet)
{
  // X addr,length:XX...
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  reg_t address = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E20");
  iter++;
  reg_t length = consume_hex_number(iter, packet.end());
  if (*iter != ':')
    return send_packet("E21");
  iter++;

  if (length == 0) {
    return send_packet("OK");
  }

  unsigned char *data = new unsigned char[length];
  for (unsigned int i = 0; i < length; i++) {
    if (iter == packet.end()) {
      return send_packet("E22");
    }
    uint8_t c = *iter;
    iter++;
    if (c == '}') {
      // The binary data representation uses 7d (ascii ‘}’) as an escape
      // character. Any escaped byte is transmitted as the escape character
      // followed by the original character XORed with 0x20. For example, the
      // byte 0x7d would be transmitted as the two bytes 0x7d 0x5d. The bytes
      // 0x23 (ascii ‘#’), 0x24 (ascii ‘$’), and 0x7d (ascii ‘}’) must always
      // be escaped.
      if (iter == packet.end()) {
        return send_packet("E23");
      }
      c = (*iter) ^ 0x20;
      iter++;
    }
    data[i] = c;
  }
  if (*iter != '#')
    return send_packet("E4b"); // EOVERFLOW

  add_operation(new collect_translation_info_op_t(*this, address, length));
  add_operation(new memory_write_op_t(*this, address, length, data));
}

void gdbserver_t::handle_continue(const std::vector<uint8_t> &packet)
{
  // c [addr]
  processor_t *p = sim->get_core(0);
  if (packet[2] != '#') {
    std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
    dpc = consume_hex_number(iter, packet.end());
    if (*iter != '#')
      return send_packet("E30");
  }

  add_operation(new continue_op_t(*this, false));
}

void gdbserver_t::handle_step(const std::vector<uint8_t> &packet)
{
  // s [addr]
  if (packet[2] != '#') {
    std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
    die("handle_step");
    //p->state.pc = consume_hex_number(iter, packet.end());
    if (*iter != '#')
      return send_packet("E40");
  }

  add_operation(new continue_op_t(*this, true));
}

void gdbserver_t::handle_kill(const std::vector<uint8_t> &packet)
{
  // k
  // The exact effect of this packet is not specified.
  // Looks like OpenOCD disconnects?
  // TODO
}

void gdbserver_t::handle_extended(const std::vector<uint8_t> &packet)
{
  // Enable extended mode. In extended mode, the remote server is made
  // persistent. The ‘R’ packet is used to restart the program being debugged.
  send_packet("OK");
  extended_mode = true;
}

void gdbserver_t::handle_breakpoint(const std::vector<uint8_t> &packet)
{
  // insert: Z type,addr,kind
  // remove: z type,addr,kind

  software_breakpoint_t bp;
  bool insert = (packet[1] == 'Z');
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  int type = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E50");
  iter++;
  bp.address = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E51");
  iter++;
  bp.size = consume_hex_number(iter, packet.end());
  // There may be more options after a ; here, but we don't support that.
  if (*iter != '#')
    return send_packet("E52");

  if (type != 0) {
    // Only software breakpoints are supported.
    return send_packet("");
  }

  if (bp.size != 2 && bp.size != 4) {
    return send_packet("E53");
  }

  fence_i_required = true;
  add_operation(new collect_translation_info_op_t(*this, bp.address, bp.size));
  if (insert) {
    unsigned char* swbp = new unsigned char[4];
    if (bp.size == 2) {
      swbp[0] = C_EBREAK & 0xff;
      swbp[1] = (C_EBREAK >> 8) & 0xff;
    } else {
      swbp[0] = EBREAK & 0xff;
      swbp[1] = (EBREAK >> 8) & 0xff;
      swbp[2] = (EBREAK >> 16) & 0xff;
      swbp[3] = (EBREAK >> 24) & 0xff;
    }

    breakpoints[bp.address] = new software_breakpoint_t(bp);
    add_operation(new memory_read_op_t(*this, bp.address, bp.size,
          breakpoints[bp.address]->instruction));
    add_operation(new memory_write_op_t(*this, bp.address, bp.size, swbp));

  } else {
    software_breakpoint_t *found_bp;
    found_bp = breakpoints[bp.address];
    unsigned char* instruction = new unsigned char[4];
    memcpy(instruction, found_bp->instruction, 4);
    add_operation(new memory_write_op_t(*this, found_bp->address,
          found_bp->size, instruction));
    breakpoints.erase(bp.address);
    delete found_bp;
  }

  return send_packet("OK");
}

void gdbserver_t::handle_query(const std::vector<uint8_t> &packet)
{
  std::string name;
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;

  consume_string(name, iter, packet.end(), ':');
  if (iter != packet.end())
    iter++;
  if (name == "Supported") {
    start_packet();
    while (iter != packet.end()) {
      std::string feature;
      consume_string(feature, iter, packet.end(), ';');
      if (iter != packet.end())
        iter++;
      if (feature == "swbreak+") {
        send("swbreak+;");
      }
    }
    send("PacketSize=131072;");
    return end_packet();
  }

  D(fprintf(stderr, "Unsupported query %s\n", name.c_str()));
  return send_packet("");
}

void gdbserver_t::handle_packet(const std::vector<uint8_t> &packet)
{
  if (compute_checksum(packet) != extract_checksum(packet)) {
    fprintf(stderr, "Received %ld-byte packet with invalid checksum\n", packet.size());
    fprintf(stderr, "Computed checksum: %x\n", compute_checksum(packet));
    print_packet(packet);
    send("-");
    return;
  }

  D(fprintf(stderr, "Received %ld-byte packet from debug client: ", packet.size()));
  D(print_packet(packet));
  send("+");

  switch (packet[1]) {
    case '!':
      return handle_extended(packet);
    case '?':
      return handle_halt_reason(packet);
    case 'g':
      return handle_general_registers_read(packet);
//    case 'k':
//      return handle_kill(packet);
    case 'm':
      return handle_memory_read(packet);
//    case 'M':
//      return handle_memory_write(packet);
    case 'X':
      return handle_memory_binary_write(packet);
    case 'p':
      return handle_register_read(packet);
    case 'P':
      return handle_register_write(packet);
    case 'c':
      return handle_continue(packet);
    case 's':
      return handle_step(packet);
    case 'z':
    case 'Z':
      return handle_breakpoint(packet);
    case 'q':
    case 'Q':
      return handle_query(packet);
  }

  // Not supported.
  D(fprintf(stderr, "** Unsupported packet: "));
  D(print_packet(packet));
  send_packet("");
}

void gdbserver_t::handle_interrupt()
{
  processor_t *p = sim->get_core(0);
  add_operation(new halt_op_t(*this, true));
}

void gdbserver_t::handle()
{
  if (client_fd > 0) {
    processor_t *p = sim->get_core(0);

    bool interrupt = sim->debug_module.get_interrupt(0);

    if (!interrupt && !operation_queue.empty()) {
      operation_t *operation = operation_queue.front();
      if (operation->step()) {
        operation_queue.pop();
        delete operation;
      }
    }

    bool halt_notification = sim->debug_module.get_halt_notification(0);
    if (halt_notification) {
      sim->debug_module.clear_halt_notification(0);
      add_operation(new halt_op_t(*this, true));
    }

    this->read();
    this->write();

  } else {
    this->accept();
  }

  if (operation_queue.empty()) {
    this->process_requests();
  }
}

void gdbserver_t::send(const char* msg)
{
  unsigned int length = strlen(msg);
  for (const char *c = msg; *c; c++)
    running_checksum += *c;
  send_buf.append((const uint8_t *) msg, length);
}

void gdbserver_t::send(uint64_t value)
{
  char buffer[3];
  for (unsigned int i = 0; i < 8; i++) {
    sprintf(buffer, "%02x", (int) (value & 0xff));
    send(buffer);
    value >>= 8;
  }
}

void gdbserver_t::send(uint32_t value)
{
  char buffer[3];
  for (unsigned int i = 0; i < 4; i++) {
    sprintf(buffer, "%02x", (int) (value & 0xff));
    send(buffer);
    value >>= 8;
  }
}

void gdbserver_t::send_packet(const char* data)
{
  start_packet();
  send(data);
  end_packet();
  expect_ack = true;
}

void gdbserver_t::start_packet()
{
  send("$");
  running_checksum = 0;
}

void gdbserver_t::end_packet(const char* data)
{
  if (data) {
    send(data);
  }

  char checksum_string[4];
  sprintf(checksum_string, "#%02x", running_checksum);
  send(checksum_string);
  expect_ack = true;
}
