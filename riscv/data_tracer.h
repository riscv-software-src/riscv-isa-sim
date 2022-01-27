// See LICENSE for license details.

#ifndef _RISCV_DATA_TRACER_H
#define _RISCV_DATA_TRACER_H

#include "proc_trace.h"
#include "memtracer.h"

class datatracer_t : public memtracer_t
{
 public:
    datatracer_t() {}
    bool interested_in_range(uint64_t begin, uint64_t end, access_type type) {
        return type == LOAD || type == STORE;
    }
    void trace(uint64_t addr, size_t bytes, access_type type) {
        (void)addr;
        (void)type;
        assert(bytes <= 8);
    }
};

//extern std::vector<datatracer_t*> d_tracers;

#endif
