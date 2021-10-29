// See LICENSE for license details.

#ifndef _RISCV_UST_TRACER_H
#define _RISCV_UST_TRACER_H

#include "disasm.h"
#include "processor.h"

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
    DATA_SRC_MEMTRACER = 4,
} data_src_t;

void ust_open_i_trace(const char *instruction_trace_file);
void ust_open_d_trace(const char *data_trace_file, bool data_trace_debug);
void ust_start(void);
void ust_step(void);
void ust_close(void);
void ust_set_addr(reg_t addr);
void ust_set_insn(insn_t insn, uint64_t binary);
void ust_set_priv(uint8_t prv);
void ust_set_exception(reg_t cause);
void ust_set_tval(reg_t tval);
void ust_set_interrupt(uint8_t interrupt);
void ust_set_is_32bit_isa(bool is_32bit_isa);
void ust_record_csr_get_trace(int which, insn_t insn, bool write, reg_t load_data, size_t size);
void ust_record_csr_set_trace(int which, reg_t store_data);
void ust_record_load(reg_t addr, uint64_t res, size_t size, data_src_t src);
void ust_record_store(reg_t addr, uint64_t val, size_t size, data_src_t src);
void ust_record_amo(reg_t addr, uint64_t lhs, uint64_t f_lhs, size_t size);
void ust_check_data_trace();

extern FILE* d_trace_fd;

#endif
