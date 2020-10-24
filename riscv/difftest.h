#ifndef _DIFFTEST_H
#define _DIFFTEST_H

#include "decode.h"
#include <iostream>

struct difftest_sim_state_t {
    reg_t regs[32];
    reg_t pc;
    reg_t npc;
    reg_t priv;
    reg_t mideleg;
    reg_t medeleg;
};

struct difftest_emu_state_t {
    reg_t regs[32];
    reg_t pc;
    reg_t inst;
    bool valid;
    bool interrupt;
    reg_t poweroff;
};

#endif