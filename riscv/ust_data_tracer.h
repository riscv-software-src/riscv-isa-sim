// See LICENSE for license details.

#ifndef _RISCV_UST_DATA_TRACER_H
#define _RISCV_UST_DATA_TRACER_H

#include "ust_tracer.h"
#include "memtracer.h"

class data_tracer_t : public memtracer_t
{
 public:
    data_tracer_t() {}
    bool interested_in_range(uint64_t begin, uint64_t end, access_type type) {
        return type == LOAD || type == STORE;
    }
    void trace(uint64_t addr, size_t len_bytes, access_type type, const uint8_t* bytes) {
        assert(bytes != NULL);
        assert(len_bytes <= 8);
        uint64_t val = (uint64_t)bytes[0];
        for (size_t i = 1; i < len_bytes; ++i)
            val |= ((uint64_t)bytes[i] << (i * 8));
        if (type == LOAD)
            ust_record_load(addr, val, len_bytes, DATA_SRC_MEMTRACER);
        else if (type == STORE)
            ust_record_store(addr, val, len_bytes, DATA_SRC_MEMTRACER);
    }
};

extern std::vector<data_tracer_t*> d_tracers;

#endif
