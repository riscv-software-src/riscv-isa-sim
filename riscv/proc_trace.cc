// See LICENSE for license details.

#include "proc_trace.h"
#include "trap.h"

#include <cstdio>
#include <cassert>

proc_trace_t::proc_trace_t(void)
    : m_itrace_fd(NULL), m_addr(0), m_insn_binary(0), m_prv(0), m_ex(0), m_ex_cause(0),
      m_tval(0), m_interrupt(0), m_is_32bit_isa(false), m_is_first_step(true),
      m_dtrace_fd(NULL), m_data_trace_debug(false), m_has_insn(false), m_insn(insn_t()),
      m_is_load(false), m_is_store(false), m_is_amo(false), m_dtype(DTYPE_INVALID),
      m_recorded_dtype(DTYPE_INVALID), m_csr_which(CSR_INVALID), m_csr_write(false),
      m_csr_load_data(0), m_csr_size(0)
{
}

void proc_trace_t::open_i_trace(const char *instruction_trace_file_name) {
    m_itrace_fd = fopen(instruction_trace_file_name, "w");
    if (!m_itrace_fd)
        fprintf(stderr, "Failed to open instruction trace file %s: %s",
                instruction_trace_file_name, strerror(errno));
    else
        fprintf(m_itrace_fd, "VALID,ADDRESS,INSN,PRIVILEGE,EXCEPTION,ECAUSE,TVAL,INTERRUPT\n");
}

/*
  Data trace of load/store is complicated by the fact that spike has an MMU and the default
  is not to perform memory trace. It is also complicated by the fact that the memtracer
  in spike does not trace I/O regions (I assume because it was designed for cache tracing and
  I/O would not be cached).
  IMPORTANT NOTE: the memtracer MUST be registered even though it is not used for producing
  the trace data. The MMU changes it's behaviour when a memtracer is registered and this is
  required.
*/
void proc_trace_t::open_d_trace(const char *data_trace_file_name, bool debug) {
    m_data_trace_debug = debug;
    if (m_data_trace_debug)
        fprintf(stderr, "Warning: data trace debug switched on - data trace files will not be usable\n");
    m_dtrace_fd = fopen(data_trace_file_name, "w");
    if (!m_dtrace_fd)
        fprintf(stderr, "Failed to open data trace file %s: %s",
                data_trace_file_name, strerror(errno));
    else
        fprintf(m_dtrace_fd, "DRETIRE,DTYPE,DADDR,DSIZE,DATA\n");
}

void proc_trace_t::step(void) {
    if (!m_itrace_fd)
        return;

    if (m_is_first_step) {
        m_is_first_step = false;
        return;
    }

    reg_t addr = m_addr;
    reg_t tval = m_tval;
    if (m_is_32bit_isa) {
        addr &= 0xffffffff;
        tval &= 0xffffffff;
    }
    fprintf(m_itrace_fd, "1,%lx,%lx,%x,%x,%lx,%lx,%x\n",
            addr, m_insn_binary, m_prv, m_ex, m_ex_cause, tval, m_interrupt);
    fflush(m_itrace_fd);

    m_ex = 0;
    m_ex_cause = 0;
    m_tval = 0;
    m_interrupt = 0;
}

void proc_trace_t::set_addr(reg_t addr) {
    m_addr = addr;
}

void proc_trace_t::set_insn(insn_t insn, uint64_t binary) {
    m_has_insn = true;
    m_insn = insn;
    m_insn_binary = binary;
    if (m_dtrace_fd) {
        m_dtype = DTYPE_INVALID;
        m_recorded_dtype = DTYPE_INVALID;
        m_csr_which = CSR_INVALID;
        m_csr_write = false;
        m_csr_load_data = 0;
        m_csr_size = 0;

        bool is_compact = (binary & 0x3) != 0x3;
        m_is_amo = false;
        if (is_compact) {
            // Not interested in load immediate
            uint64_t inst_15_13 = (binary >> 13) & ((insn_bits_t(1) << 3) - 1);
            uint64_t inst_1_0 = binary & 0x3;
            m_is_load = (((inst_1_0 == 0) || (inst_1_0 == 2)) && (inst_15_13 > 0) && (inst_15_13 < 4));
            m_is_store = ((inst_1_0 == 0) || (inst_1_0 == 2)) & (inst_15_13 > 4);
        } else {
            uint64_t inst_6_0 = binary & ((insn_bits_t(1) << 7) - 1);
            uint64_t inst_4_2 = binary >> 2 & ((insn_bits_t(1) << 3) - 1);
            uint64_t inst_6_5 = binary >> 5 & ((insn_bits_t(1) << 2) - 1);
            m_is_load = (inst_4_2 < 2) && (inst_6_5 == 0);
            m_is_store = (inst_4_2 < 2) && (inst_6_5 == 1);
            m_is_amo = (inst_6_0 == 0x2f);
        }

        if (m_is_load)
            set_dtype(DTYPE_LOAD);
        if (m_is_store)
            set_dtype(DTYPE_STORE);
        if (m_data_trace_debug)
            fprintf(m_dtrace_fd, "DEBUG: INSN@%lx: %lx\n", m_addr, m_insn_binary);
    }
}

void proc_trace_t::set_priv(uint8_t prv) {
    m_prv = prv;
}

void proc_trace_t::set_exception(reg_t cause) {
    m_ex = 1;
    m_ex_cause = cause;
}

void proc_trace_t::set_tval(reg_t tval) {
    m_tval = tval;
}

void proc_trace_t::set_interrupt(uint8_t interrupt) {
    m_interrupt = interrupt;
}

void proc_trace_t::set_is_32bit_isa(bool is_32bit_isa) {
    m_is_32bit_isa = is_32bit_isa;
}

void proc_trace_t::record_load(reg_t addr, uint64_t res, size_t size, data_src_t src) {
    if (m_dtrace_fd) {
        if (!m_has_insn || m_is_amo)
            return;

        if (!m_is_load) {
            // It's not clear why these are being triggered.
            // They seem to come from DATA_SRC_LOAD_TLB_1
            if (m_data_trace_debug)
                fprintf(m_dtrace_fd, "NOT LOAD SRC %d %lx ld %d st %d amo %d %lx\n",
                        src, addr, m_is_load, m_is_store, m_is_amo, m_insn_binary);
            else
                fprintf(stderr, "NOT LOAD SRC %d %lx ld %d st %d amo %d %lx\n",
                        src, addr, m_is_load, m_is_store, m_is_amo, m_insn_binary);
            return;
        }

        // Limit value to the size given
        if (size != 8)
            res &= ((uint64_t(1) << (size * 8)) - 1);

        assert(m_is_load);
        assert(m_dtype == DTYPE_LOAD);
        record_data(addr, (uint8_t*)&res, size);
    }
}

void proc_trace_t::record_store(reg_t addr, uint64_t val, size_t size, data_src_t src) {
    if (m_dtrace_fd) {
        if (!m_has_insn || m_is_amo)
            return;

        // Limit value to the size given
        if (size != 8)
            val &= ((uint64_t(1) << (size * 8)) - 1);

        assert(m_is_store);
        assert(m_dtype == DTYPE_STORE);
        record_data(addr, (uint8_t*)&val, size);
    }
}

/* Atomic and CSR accesses have either both load and store data, or store data and an operand. For
   CSRs and unified atomics, both values are reported via data, with the store data in the LSBs and
   the load data or operand in the MSBs. */
void proc_trace_t::record_amo(reg_t addr, uint64_t load_data, uint64_t store_data, size_t size) {
    if (m_dtrace_fd) {
        reg_t insn_31_27 = (m_insn_binary >> 27) & ((insn_bits_t(1) << 5) - 1);
        switch (insn_31_27) {
        case 0: set_dtype(DTYPE_ATOMIC_ADD); break;
        case 1: set_dtype(DTYPE_ATOMIC_SWAP); break;
        case 4: set_dtype(DTYPE_ATOMIC_XOR); break;
        case 8: set_dtype(DTYPE_ATOMIC_OR); break;
        case 12: set_dtype(DTYPE_ATOMIC_AND); break;
        case 16: case 24: set_dtype(DTYPE_ATOMIC_MIN); break;
        case 20: case 28: set_dtype(DTYPE_ATOMIC_MAX); break;
        default: assert(0); break;
        }

        uint64_t pair[2] = {load_data, store_data};
        record_data(addr, (uint8_t*)pair, 2 * size);
    }
}

void proc_trace_t::record_csr_get(int which, insn_t insn, bool write, reg_t load_data, size_t size) {
    // Record the current csr being processed as there may be a set to go with this
    m_csr_which = which;
    m_csr_write = write;
    m_csr_load_data = load_data;
    m_csr_size = size;

    reg_t func3 = insn.rm();
    switch (func3) {
    case 1: // CSRRW
    case 5: // CSRRWI
        set_dtype(DTYPE_CSR_READ_WRITE);
        break;
    case 2: // CSRRS
    case 6: // CSRRSI
        set_dtype(DTYPE_CSR_READ_SET);
        break;
    case 3: // CSRRC
    case 7: // CSRRCI
        set_dtype(DTYPE_CSR_READ_CLEAR);
        break;
    default:
        break;
    }

    // If not being written then the trace must be written here
    if (!m_csr_write) {
        assert(m_csr_which != CSR_INVALID);
        uint64_t pair[2] = {load_data, 0};
        record_data(m_csr_which, (uint8_t*)pair, 2 * size);
    }
}

void proc_trace_t::record_csr_set(int which, reg_t store_data) {
    // mret is a special case where there is no call to csr_get to go with this.
    // In this case the csr must be MSTATUS and it does not get recorded.
    if (m_insn_binary == 0x30200073) {
        assert(which == CSR_MSTATUS);
        return;
    } else {
        assert(which == m_csr_which);
        assert(m_csr_write);
    }
    assert(m_csr_which != CSR_INVALID);
    assert(m_csr_size != 0);
    uint64_t pair[2] = {m_csr_load_data, store_data};
    record_data(m_csr_which, (uint8_t*)pair, 2 * m_csr_size);
}

void proc_trace_t::check_data_trace(void) {
    m_has_insn = false;
    if (m_dtrace_fd) {
        if (!m_is_amo) {
            bool check_passed = true;
            if (m_is_load && m_recorded_dtype != DTYPE_LOAD) {
                fprintf(stderr, "Expected load but not recorded @0x%lx insn:%lx\n", m_addr, m_insn_binary);
                check_passed = false;
            }
            if (m_is_store && (m_recorded_dtype != DTYPE_STORE)) {
                fprintf(stderr, "Expected store but not recorded @0x%lx insn:%lx\n", m_addr, m_insn_binary);
                check_passed = false;
            }
            if (!m_is_load && (m_recorded_dtype == DTYPE_LOAD)) {
                fprintf(stderr, "Load recorded but not expected @0x%lx insn:%lx\n", m_addr, m_insn_binary);
                check_passed = false;
            }
            if (!m_is_store && (m_recorded_dtype == DTYPE_STORE)) {
                fprintf(stderr, "Store recorded but not expected @0x%lx insn:%lx\n", m_addr, m_insn_binary);
                check_passed = false;
            }
            assert(check_passed);
        }

        assert((m_csr_which == CSR_INVALID) || (m_recorded_dtype == DTYPE_CSR_READ_WRITE) ||
               (m_recorded_dtype == DTYPE_CSR_READ_SET) || (m_recorded_dtype == DTYPE_CSR_READ_CLEAR));
    }
}

void proc_trace_t::close(void) {
    if (m_itrace_fd)
        fclose(m_itrace_fd);
    m_itrace_fd = NULL;
    if (m_dtrace_fd)
        fclose(m_dtrace_fd);
    m_dtrace_fd = NULL;
}

void proc_trace_t::set_dtype(dtype_t dtype) {
    assert(m_dtype == DTYPE_INVALID);
    m_dtype = dtype;
}

void proc_trace_t::record_data(reg_t addr, uint8_t* data, size_t size) {
    assert(m_dtype != DTYPE_INVALID);
    if (m_recorded_dtype != DTYPE_INVALID) {
        if (m_recorded_dtype == m_dtype)
            fprintf(stderr, "Already recorded dtype %d duplicate\n", m_recorded_dtype);
        else
            fprintf(stderr, "Already recorded dtype %d now attempting %d\n", m_recorded_dtype, m_dtype);
        assert(0);
        return;
    }
    m_recorded_dtype = m_dtype;

    if (m_is_32bit_isa) {
        addr &= 0xffffffff;
    }

    assert(size != 0);
    size_t dsize = 0;
    size_t _size = size;
    while (_size >>= 1)
        ++dsize;
    fprintf(m_dtrace_fd, "1,%d,%lx,%ld,", m_dtype, addr, dsize);
    size_t i = size - 1;
    while ((i > 0) && (data[i] == 0))
        --i;
    fprintf(m_dtrace_fd, "%hx", data[i]);
    while (i > 0) {
        --i;
        fprintf(m_dtrace_fd, "%02hx", data[i]);
    }
    fprintf(m_dtrace_fd, "\n");
    fflush(m_dtrace_fd);
}
