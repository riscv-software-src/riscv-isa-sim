#include "devices.h"
#include "processor.h"

mu500_t::mu500_t(std::vector<processor_t*>&) {
}

bool mu500_t::load(reg_t addr, size_t len, uint8_t* bytes) {
    return true;
}

bool mu500_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
    std::cerr << "accessed " << addr << " : " << len << " : " << (int)(*bytes) << std::endl;
    return true;
}
