// See LICENSE for license details.

#ifndef _RISCV_PROC_TRACE_H
#define _RISCV_PROC_TRACE_H

#include <fstream>

#include "disasm.h"
#include "processor.h"
#include "data_tracer.h"

typedef enum {
    DTYPE_LOAD = 0,
    DTYPE_STORE = 1,
    DTYPE_CSR_READ_WRITE = 4,
    DTYPE_CSR_READ_SET = 5,
    DTYPE_CSR_READ_CLEAR = 6,
    DTYPE_ATOMIC_SWAP = 8,
    DTYPE_ATOMIC_ADD = 9,
    DTYPE_ATOMIC_AND = 10,
    DTYPE_ATOMIC_OR = 11,
    DTYPE_ATOMIC_XOR = 12,
    DTYPE_ATOMIC_MAX = 13,
    DTYPE_ATOMIC_MIN = 14,
    DTYPE_STORE_FAILURE = 15,
    DTYPE_INVALID = 16,
} dtype_t;

typedef enum {
    DATA_SRC_LOAD_SLOW_PATH = 0,
    DATA_SRC_LOAD_TLB_1 = 1,
    DATA_SRC_LOAD_TLB_2 = 2,
    DATA_SRC_STORE = 3,
} data_src_t;

#define CSR_INVALID (-1)

class proc_trace_t
{
public:
    proc_trace_t(void);

    void open_i_trace(const char *instruction_trace_file_name);
    void open_d_trace(const char *data_trace_file_name);

    void step(void);
    void set_addr(reg_t addr);
    void set_insn(insn_t insn, uint64_t binary);

    void set_priv(uint8_t prv);
    void set_exception(reg_t cause);
    void set_tval(reg_t tval);
    void set_interrupt(uint8_t interrupt);
    void set_is_32bit_isa(bool is_32bit_isa);

    void record_load(reg_t addr, uint64_t res, size_t size, data_src_t src);
    void record_store(reg_t addr, uint64_t val, size_t size, data_src_t src);
    void record_amo(reg_t addr, uint64_t load_data, uint64_t store_data, size_t size);
    void record_csr_get(int which, insn_t insn, bool write, reg_t load_data, size_t size);
    void record_csr_set(int which, reg_t store_data);
    void check_data_trace(void);

    void close(void);

private:
    // Instruction trace information
    std::ofstream m_itrace_ofs;
    reg_t m_addr = 0;
    uint64_t m_insn_binary = 0;
    uint8_t m_prv = 0;
    uint8_t m_ex = 0;
    reg_t m_ex_cause = 0;
    reg_t m_tval = 0;
    uint8_t m_interrupt = 0;
    bool m_is_32bit_isa = false;
    bool m_is_first_step = true;

    // Data trace information
    std::ofstream m_dtrace_ofs;
    std::vector<datatracer_t*> m_d_tracers;

    insn_t m_insn = insn_t();
    bool m_is_amo = false;

    dtype_t m_dtype = DTYPE_INVALID;

    int m_csr_which = CSR_INVALID;
    bool m_csr_write = false;
    uint64_t m_csr_load_data = 0;
    size_t m_csr_size = 0;

    void set_dtype(dtype_t dtype);
    void record_data(reg_t addr, uint8_t* data, size_t size);
};

#endif // _RISCV_PROC_TRACE_H
