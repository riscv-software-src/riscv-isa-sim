#ifndef _DIFFTEST_H
#define _DIFFTEST_H

#include "decode.h"
#include <iostream>

struct difftest_regs_t {
    reg_t regs[32];
    reg_t pc;
    reg_t npc;
};

#endif