// See LICENSE for license details.

#include "ust_tracer.h"
#include "ust_data_tracer.h"
#include "trap.h"

#include <cstdio>
#include <cassert>

// Instruction trace information
static reg_t s_addr = 0;
static uint64_t s_insn_binary = 0;
static uint8_t s_prv = 0;
static uint8_t s_ex = 0;
static reg_t s_ex_cause = 0;
static reg_t s_tval = 0;
static uint8_t s_interrupt = 0;
static bool s_is_32bit_isa = false;
static FILE* i_trace_fd = NULL;
static bool s_is_first_step = true;

// Data trace information
FILE* d_trace_fd = NULL;
std::vector<data_tracer_t*> d_tracers;
static bool s_data_trace_debug = false;

static bool s_has_insn = false;
static insn_t s_insn = insn_t();
static bool s_is_load = false;
static bool s_is_store = false;
static bool s_is_amo = false;

static dtype_t s_dtype = DTYPE_INVALID;
static dtype_t s_recorded_dtype = DTYPE_INVALID;

static bool s_has_check_data = false;
static uint64_t s_check_data = 0;

#define CSR_INVALID (-1)
static int s_csr_which = CSR_INVALID;
static bool s_csr_write = false;
static uint64_t s_csr_load_data = 0;
static size_t s_csr_size = 0;

void ust_open_i_trace(const char *instruction_trace_file)
{
  i_trace_fd = fopen(instruction_trace_file, "w");
  if (!i_trace_fd)
    fprintf(stderr, "Failed to open instruction trace file %s: %s",
            instruction_trace_file, strerror(errno));
  else
    fprintf(i_trace_fd, "VALID,ADDRESS,INSN,PRIVILEGE,EXCEPTION,ECAUSE,TVAL,INTERRUPT\n");
}

/*
  Data trace of load/store is complicated by the fact that spike has an MMU and the default
  is not to perform memory trace. It is also complicated by the fact that the memtracer
  in spike does not trace I/O regions (I assume because it was designed for cache tracing and
  I/O would not be cached).
  IMPORTANT NOTE: the memtracer MUST be registered even though it is not used for producing
  the trace data. The MMU changes it's behaviour when a memtracer is registered and this is
  required.

  memtracer: this traces load/stores that are not in I/O regions
  mmu direct: this inserts a new function call into the load_func and store_func templates in mmu.h

  **Note that the mmu direct trace now outputs virtual addresses whereas the memtracer outputs
  physical addresses.**
 */
void ust_open_d_trace(const char *data_trace_file, bool data_trace_debug)
{
  s_data_trace_debug = data_trace_debug;
  if (s_data_trace_debug)
      fprintf(stderr, "Warning: data trace debug switched on - data trace files will not be usable\n");
  d_trace_fd = fopen(data_trace_file, "w");
  if (!d_trace_fd)
    fprintf(stderr, "Failed to open data trace file %s: %s",
            data_trace_file, strerror(errno));
  else
    fprintf(d_trace_fd, "DRETIRE,DTYPE,DADDR,DSIZE,DATA\n");
}

void ust_step(void)
{
  if (!i_trace_fd)
    return;

  if (s_is_first_step) {
      s_is_first_step = false;
      return;
  }

  reg_t addr = s_addr;
  reg_t tval = s_tval;
  if (s_is_32bit_isa) {
    addr &= 0xffffffff;
    tval &= 0xffffffff;
  }
  fprintf(i_trace_fd, "1,%lx,%lx,%x,%x,%lx,%lx,%x\n",
          addr, s_insn_binary, s_prv, s_ex, s_ex_cause, tval, s_interrupt);
  fflush(i_trace_fd);

  s_ex = 0;
  s_ex_cause = 0;
  s_tval = 0;
  s_interrupt = 0;
}

void ust_close(void)
{
  if (i_trace_fd)
    fclose(i_trace_fd);
  i_trace_fd = NULL;
  if (d_trace_fd)
    fclose(d_trace_fd);
  d_trace_fd = NULL;
}

void ust_set_addr(reg_t addr) {
  s_addr = addr;
}

static void ust_set_dtype(dtype_t dtype) {
    assert(s_dtype == DTYPE_INVALID);
    s_dtype = dtype;
}

static void ust_record_data(reg_t addr, uint8_t* data, size_t size) {
    assert(s_dtype != DTYPE_INVALID);
    if (s_recorded_dtype != DTYPE_INVALID) {
        if (s_recorded_dtype == s_dtype)
            fprintf(stderr, "Already recorded dtype %d duplicate\n", s_recorded_dtype);
        else
            fprintf(stderr, "Already recorded dtype %d now attempting %d\n", s_recorded_dtype, s_dtype);
        assert(0);
        return;
    }
    s_recorded_dtype = s_dtype;

    if (s_is_32bit_isa) {
        addr &= 0xffffffff;
    }

    assert(size != 0);
    size_t dsize = 0;
    size_t _size = size;
    while (_size >>= 1) ++dsize;
    fprintf(d_trace_fd, "1,%d,%lx,%ld,", s_dtype, addr, dsize);
    size_t i = size - 1;
    while ((i > 0) && (data[i] == 0))
        --i;
    fprintf(d_trace_fd, "%hx", data[i]);
    while (i > 0) {
        --i;
        fprintf(d_trace_fd, "%02hx", data[i]);
    }
    fprintf(d_trace_fd, "\n");
    fflush(d_trace_fd);
}

void ust_set_insn(insn_t insn, uint64_t binary) {
  s_has_insn = true;
  s_insn = insn;
  s_insn_binary = binary;
  if (d_trace_fd) {
      s_dtype = DTYPE_INVALID;
      s_recorded_dtype = DTYPE_INVALID;
      s_has_check_data = false;
      s_check_data = 0;
      s_csr_which = CSR_INVALID;
      s_csr_write = false;
      s_csr_load_data = 0;
      s_csr_size = 0;

      bool is_compact = (binary & 0x3) != 0x3;
      s_is_amo = false;
      if (is_compact) {
          // Not interested in load immediate
          uint64_t inst_15_13 = (binary >> 13) & ((insn_bits_t(1) << 3) - 1);
          uint64_t inst_1_0 = binary & 0x3;
          s_is_load = (((inst_1_0 == 0) || (inst_1_0 == 2)) && (inst_15_13 > 0) && (inst_15_13 < 4));
          s_is_store = ((inst_1_0 == 0) || (inst_1_0 == 2)) & (inst_15_13 > 4);
      } else {
          uint64_t inst_6_0 = binary & ((insn_bits_t(1) << 7) - 1);
          uint64_t inst_4_2 = binary >> 2 & ((insn_bits_t(1) << 3) - 1);
          uint64_t inst_6_5 = binary >> 5 & ((insn_bits_t(1) << 2) - 1);
          s_is_load = (inst_4_2 < 2) && (inst_6_5 == 0);
          s_is_store = (inst_4_2 < 2) && (inst_6_5 == 1);
          s_is_amo = (inst_6_0 == 0x2f);
      }

      if (s_is_load)
          ust_set_dtype(DTYPE_LOAD);
      if (s_is_store)
          ust_set_dtype(DTYPE_STORE);
      if (s_data_trace_debug)
          fprintf(d_trace_fd, "DEBUG: INSN@%lx: %lx\n", s_addr, s_insn_binary);
  }
}

void ust_set_priv(uint8_t prv) {
  s_prv = prv;
}

void ust_set_exception(reg_t cause) {
  s_ex = 1;
  s_ex_cause = cause;
}

void ust_set_tval(reg_t tval) {
  s_tval = tval;
}

void ust_set_interrupt(uint8_t interrupt) {
  s_interrupt = interrupt;
}

void ust_set_is_32bit_isa(bool is_32bit_isa) {
    s_is_32bit_isa = is_32bit_isa;
}

void ust_record_load(reg_t addr, uint64_t res, size_t size, data_src_t src) {
    if (d_trace_fd) {
        if (!s_has_insn || s_is_amo)
            return;

        if (!s_is_load) {
            // It's not clear why these are being triggered. They seem to come from DATA_SRC_LOAD_TLB_1
            if (s_data_trace_debug)
                fprintf(d_trace_fd, "NOT LOAD SRC %d %lx ld %d st %d amo %d %lx\n", src, addr, s_is_load, s_is_store, s_is_amo, s_insn_binary);
            else
                fprintf(stderr, "NOT LOAD SRC %d %lx ld %d st %d amo %d %lx\n", src, addr, s_is_load, s_is_store, s_is_amo, s_insn_binary);
            return;
        }

        // Limit value to the size given
        if (size != 8)
            res &= ((uint64_t(1) << (size * 8)) - 1);

        if (s_has_check_data)
            assert(res == s_check_data);
        else {
            s_has_check_data = true;
            s_check_data = res;
        }

        if (src == DATA_SRC_MEMTRACER)
            // Make sure we only record one set of data, but we can still do the above sanity check
            // on the data.
            return;

        assert(s_is_load);
        assert(s_dtype == DTYPE_LOAD);
        ust_record_data(addr, (uint8_t*)&res, size);
    }
}

void ust_record_store(reg_t addr, uint64_t val, size_t size, data_src_t src) {
    if (d_trace_fd) {
        if (!s_has_insn || s_is_amo)
            return;

        // Limit value to the size given
        if (size != 8)
            val &= ((uint64_t(1) << (size * 8)) - 1);

        if (s_has_check_data)
            assert(val == s_check_data);
        else {
            s_has_check_data = true;
            s_check_data = val;
        }

        if (src == DATA_SRC_MEMTRACER)
            // Make sure we only record one set of data, but we can still do the above sanity check
            // on the data.
            return;

        assert(s_is_store);
        assert(s_dtype == DTYPE_STORE);
        ust_record_data(addr, (uint8_t*)&val, size);
    }
}

/* Atomic and CSR accesses have either both load and store data, or store data and an operand. For
CSRs and unified atomics, both values are reported via data, with the store data in the LSBs and
the load data or operand in the MSBs. */

void ust_record_amo(reg_t addr, uint64_t load_data, uint64_t store_data, size_t size) {
    if (d_trace_fd) {
        reg_t insn_31_27 = (s_insn_binary >> 27) & ((insn_bits_t(1) << 5) - 1);
        switch (insn_31_27) {
        case 0: ust_set_dtype(DTYPE_ATOMIC_ADD); break;
        case 1: ust_set_dtype(DTYPE_ATOMIC_SWAP); break;
        case 4: ust_set_dtype(DTYPE_ATOMIC_XOR); break;
        case 8: ust_set_dtype(DTYPE_ATOMIC_OR); break;
        case 12: ust_set_dtype(DTYPE_ATOMIC_AND); break;
        case 16: case 24: ust_set_dtype(DTYPE_ATOMIC_MIN); break;
        case 20: case 28: ust_set_dtype(DTYPE_ATOMIC_MAX); break;
        default: assert(0); break;
        }

        uint64_t pair[2] = {load_data, store_data};
        ust_record_data(addr, (uint8_t*)pair, 2 * size);
    }
}

void ust_record_csr_get_trace(int which, insn_t insn, bool write, reg_t load_data, size_t size) {
    // Record the current csr being processed as there may be a set to go with this
    s_csr_which = which;
    s_csr_write = write;
    s_csr_load_data = load_data;
    s_csr_size = size;

    reg_t func3 = insn.rm();
    switch (func3) {
    case 1: // CSRRW
    case 5: // CSRRWI
        ust_set_dtype(DTYPE_CSR_READ_WRITE);
        break;
    case 2: // CSRRS
    case 6: // CSRRSI
        ust_set_dtype(DTYPE_CSR_READ_SET);
        break;
    case 3: // CSRRC
    case 7: // CSRRCI
        ust_set_dtype(DTYPE_CSR_READ_CLEAR);
        break;
    default:
        break;
    }

    // If not being written then the trace must be written here
    if (!s_csr_write) {
        assert(s_csr_which != CSR_INVALID);
        uint64_t pair[2] = {load_data, 0};
        ust_record_data(s_csr_which, (uint8_t*)pair, 2 * size);
    }
}

void ust_record_csr_set_trace(int which, reg_t store_data) {
    // mret is a special case where there is no call to csr_get to go with this.
    // In this case the csr must be MSTATUS and it does not get recorded.
    if (s_insn_binary == 0x30200073) {
        assert(which == CSR_MSTATUS);
        return;
    } else {
        assert(which == s_csr_which);
        assert(s_csr_write);
    }
    assert(s_csr_which != CSR_INVALID);
    assert(s_csr_size != 0);
    uint64_t pair[2] = {s_csr_load_data, store_data};
    ust_record_data(s_csr_which, (uint8_t*)pair, 2 * s_csr_size);
}

void ust_check_data_trace() {
    s_has_insn = false;
    if (d_trace_fd) {
        if (!s_is_amo) {
            bool check_passed = true;
            if (s_is_load && s_recorded_dtype != DTYPE_LOAD) {
                fprintf(stderr, "Expected load but not recorded @0x%lx insn:%lx\n", s_addr, s_insn_binary);
                check_passed = false;
            }
            if (s_is_store && (s_recorded_dtype != DTYPE_STORE)) {
                fprintf(stderr, "Expected store but not recorded @0x%lx insn:%lx\n", s_addr, s_insn_binary);
                check_passed = false;
            }
            if (!s_is_load && (s_recorded_dtype == DTYPE_LOAD)) {
                fprintf(stderr, "Load recorded but not expected @0x%lx insn:%lx\n", s_addr, s_insn_binary);
                check_passed = false;
            }
            if (!s_is_store && (s_recorded_dtype == DTYPE_STORE)) {
                fprintf(stderr, "Store recorded but not expected @0x%lx insn:%lx\n", s_addr, s_insn_binary);
                check_passed = false;
            }
            assert(check_passed);
        }

        assert((s_csr_which == CSR_INVALID) || (s_recorded_dtype == DTYPE_CSR_READ_WRITE) ||
               (s_recorded_dtype == DTYPE_CSR_READ_SET) || (s_recorded_dtype == DTYPE_CSR_READ_CLEAR));
    }
}
