#ifndef _DIFFTEST_H
#define _DIFFTEST_H

#include "decode.h"
#include <iostream>

struct difftest_sim_state_t {
    reg_t regs[32];
    reg_t pc;
    reg_t npc;
};

struct difftest_emu_state_t {
    reg_t regs[32];
    reg_t pc;
    reg_t inst;
    bool valid;
    bool trmt;
};

#endif