// See LICENSE for license details.

#include "encoding.h"
#include "proc_trace.h"
#include "trap.h"

#include <fstream>
#include <iomanip>
#include <cassert>

proc_trace_t::proc_trace_t(void)
    : m_addr(0), m_insn_binary(0), m_prv(0), m_ex(0), m_ex_cause(0),
      m_tval(0), m_interrupt(0), m_is_32bit_isa(false), m_is_first_step(true),
      m_insn(insn_t()), m_is_amo(false), m_dtype(DTYPE_INVALID),
      m_csr_which(CSR_INVALID), m_csr_write(false), m_csr_load_data(0), m_csr_size(0)
{
}

void proc_trace_t::open_i_trace(const char *instruction_trace_file_name) {
    m_itrace_ofs.open(instruction_trace_file_name);
    if (!m_itrace_ofs)
        throw std::runtime_error("Failed to open instruction trace file "
                                 + std::string(instruction_trace_file_name));
    else
        m_itrace_ofs << "VALID,ADDRESS,INSN,PRIVILEGE,EXCEPTION,ECAUSE,TVAL,INTERRUPT\n";
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
void proc_trace_t::open_d_trace(const char *data_trace_file_name) {
#ifndef RISCV_ENABLE_COMMITLOG
    fputs("Commit logging support has not been properly enabled; "
          "this is required to allow processor data trace, "
          "please re-build the riscv-isa-sim project using "
          "\"configure --enable-commitlog\".\n",
          stderr);
    abort();
#endif

    m_dtrace_ofs.open(data_trace_file_name);
    if (!m_dtrace_ofs)
        throw std::runtime_error("Failed to open data trace file "
                                 + std::string(data_trace_file_name));
    else
        m_dtrace_ofs << "DRETIRE,DTYPE,DADDR,DSIZE,DATA\n";
}

void proc_trace_t::step(void) {
    if (!m_itrace_ofs)
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
    m_itrace_ofs << "1," << std::hex << addr << "," << m_insn_binary << ","
                 << static_cast<unsigned>(m_prv) << "," << static_cast<unsigned>(m_ex) << ","
                 << m_ex_cause << "," << tval << ","
                 << static_cast<unsigned>(m_interrupt) << "\n";
    m_itrace_ofs.flush();

    m_ex = 0;
    m_ex_cause = 0;
    m_tval = 0;
    m_interrupt = 0;
}

void proc_trace_t::set_addr(reg_t addr) {
    m_addr = addr;
}

void proc_trace_t::set_insn(insn_t insn, uint64_t binary) {
    m_insn = insn;
    m_insn_binary = binary;
    if (m_dtrace_ofs) {
        m_dtype = DTYPE_INVALID;
        m_csr_which = CSR_INVALID;
        m_csr_write = false;
        m_csr_load_data = 0;
        m_csr_size = 0;

        /* Atomic instructions also trigger record_load/record_store so it is necessary
           to determine the type of the instruction to allow differentiation. */
        bool is_amo_add = (((m_insn_binary & MASK_AMOADD_W) == MATCH_AMOADD_W) ||
                           ((m_insn_binary & MASK_AMOADD_D) == MATCH_AMOADD_D));
        bool is_amo_swap = (((m_insn_binary & MASK_AMOSWAP_W) == MATCH_AMOSWAP_W) ||
                            ((m_insn_binary & MASK_AMOSWAP_D) == MATCH_AMOSWAP_D));
        bool is_amo_xor = (((m_insn_binary & MASK_AMOXOR_W) == MATCH_AMOXOR_W) ||
                           ((m_insn_binary & MASK_AMOXOR_D) == MATCH_AMOXOR_D));
        bool is_amo_or = (((m_insn_binary & MASK_AMOOR_W) == MATCH_AMOOR_W) ||
                          ((m_insn_binary & MASK_AMOOR_D) == MATCH_AMOOR_D));
        bool is_amo_and = (((m_insn_binary & MASK_AMOAND_W) == MATCH_AMOAND_W) ||
                           ((m_insn_binary & MASK_AMOAND_D) == MATCH_AMOAND_D));
        bool is_amo_min = (((m_insn_binary & MASK_AMOMIN_W) == MATCH_AMOMIN_W) ||
                           ((m_insn_binary & MASK_AMOMINU_W) == MATCH_AMOMINU_W) ||
                           ((m_insn_binary & MASK_AMOMIN_D) == MATCH_AMOMIN_D) ||
                           ((m_insn_binary & MASK_AMOMINU_D) == MATCH_AMOMINU_D));
        bool is_amo_max = (((m_insn_binary & MASK_AMOMAX_W) == MATCH_AMOMAX_W) ||
                           ((m_insn_binary & MASK_AMOMAXU_W) == MATCH_AMOMAXU_W) ||
                           ((m_insn_binary & MASK_AMOMAX_D) == MATCH_AMOMAX_D) ||
                           ((m_insn_binary & MASK_AMOMAXU_D) == MATCH_AMOMAXU_D));
        m_is_amo = (is_amo_add || is_amo_swap || is_amo_xor || is_amo_or ||
                    is_amo_and || is_amo_min || is_amo_max);
        if (is_amo_add) set_dtype(DTYPE_ATOMIC_ADD);
        if (is_amo_swap) set_dtype(DTYPE_ATOMIC_SWAP);
        if (is_amo_xor) set_dtype(DTYPE_ATOMIC_XOR);
        if (is_amo_or) set_dtype(DTYPE_ATOMIC_OR);
        if (is_amo_and) set_dtype(DTYPE_ATOMIC_AND);
        if (is_amo_min) set_dtype(DTYPE_ATOMIC_MIN);
        if (is_amo_max) set_dtype(DTYPE_ATOMIC_MAX);
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
    if (m_dtrace_ofs) {
        /* Atomic instructions also trigger this function but this needs to be
           ignored in this case */
        if (!m_is_amo) {
            // Limit value to the size given
            if (size != 8)
                res &= ((uint64_t(1) << (size * 8)) - 1);

            set_dtype(DTYPE_LOAD);
            record_data(addr, (uint8_t*)&res, size);
        }
    }
}

void proc_trace_t::record_store(reg_t addr, uint64_t val, size_t size, data_src_t src) {
    if (m_dtrace_ofs) {
        /* Atomic instructions also trigger this function but this needs to be
           ignored in this case */
        if (!m_is_amo) {
            // Limit value to the size given
            if (size != 8)
                val &= ((uint64_t(1) << (size * 8)) - 1);

            set_dtype(DTYPE_STORE);
            record_data(addr, (uint8_t*)&val, size);
        }
    }
}

/* Atomic and CSR accesses have either both load and store data, or store data and an operand. For
   CSRs and unified atomics, both values are reported via data, with the store data in the LSBs and
   the load data or operand in the MSBs. */
void proc_trace_t::record_amo(reg_t addr, uint64_t load_data, uint64_t store_data, size_t size) {
    if (m_dtrace_ofs) {
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

    assert(insn.bits() == m_insn.bits());
    bool is_csrrw = ((m_insn_binary & MASK_CSRRW) == MATCH_CSRRW);
    bool is_csrrwi = ((m_insn_binary & MASK_CSRRWI) == MATCH_CSRRWI);
    bool is_csrrs = ((m_insn_binary & MASK_CSRRS) == MATCH_CSRRS);
    bool is_csrrsi = ((m_insn_binary & MASK_CSRRSI) == MATCH_CSRRSI);
    bool is_csrrc = ((m_insn_binary & MASK_CSRRC) == MATCH_CSRRC);
    bool is_csrrci = ((m_insn_binary & MASK_CSRRCI) == MATCH_CSRRCI);
    if (is_csrrw || is_csrrwi) set_dtype(DTYPE_CSR_READ_WRITE);
    if (is_csrrs || is_csrrsi) set_dtype(DTYPE_CSR_READ_SET);
    if (is_csrrc || is_csrrci) set_dtype(DTYPE_CSR_READ_CLEAR);

    // If not being written then the trace must be written here
    if (!m_csr_write) {
        assert(m_csr_which != CSR_INVALID);
        uint64_t pair[2] = {load_data, 0};
        record_data(m_csr_which, (uint8_t*)pair, 2 * size);
    }
}

void proc_trace_t::record_csr_set(int which, reg_t store_data) {
    // mret is a special case where there is no call to csr_get to go with this.
    // In this case the csr is guaranteed to be MSTATUS and it does not get recorded
    // because the trace decoder can determine this information.
    if (m_insn_binary == MATCH_MRET) {
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

void proc_trace_t::close(void) {
    if (m_itrace_ofs)
        m_itrace_ofs.close();
    if (m_dtrace_ofs)
        m_dtrace_ofs.close();
}

void proc_trace_t::set_dtype(dtype_t dtype) {
    assert(m_dtype == DTYPE_INVALID);
    m_dtype = dtype;
}

void proc_trace_t::record_data(reg_t addr, uint8_t* data, size_t size) {
    assert(m_dtype != DTYPE_INVALID);

    if (m_is_32bit_isa) {
        addr &= 0xffffffff;
    }

    assert(size != 0);
    size_t dsize = 0;
    size_t _size = size;
    while (_size >>= 1)
        ++dsize;
    m_dtrace_ofs << "1," << m_dtype << "," << std::hex << addr << "," << std::dec << dsize << ",";
    size_t i = size - 1;
    while ((i > 0) && (data[i] == 0))
        --i;
    m_dtrace_ofs << std::hex << static_cast<unsigned>(data[i]);
    while (i > 0) {
        --i;
        m_dtrace_ofs << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(data[i]);
    }
    m_dtrace_ofs << "\n";
    m_dtrace_ofs.flush();
}
