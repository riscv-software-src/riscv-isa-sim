#include "trace_ingress.h"

insn_type _get_insn_type(insn_t* insn, bool taken) {
    if (_is_branch(insn)) {
        return taken ? I_BRANCH_TAKEN : I_BRANCH_NON_TAKEN;
    }
    else if (_is_jal(insn)) {
        return I_JUMP_INFERABLE;
    }
    else if (_is_jalr(insn)) {
        return I_JUMP_UNINFERABLE;
    }
    // TODO: further categorization is not implemented for now
    else {
        return I_NONE;
    }
}

void hart_to_encoder_ingress_init(processor_t* p, hart_to_encoder_ingress_t* packet, insn_t* insn, reg_t npc) {
    // print insn
    bool taken = npc != p->get_state()->pc + insn_length(insn->bits());
    packet->i_type = _get_insn_type(insn, taken);
    packet->exc_cause = 0;
    packet->tval = 0;
    packet->priv = P_M; // TODO: check for processor privilege level
    packet->i_addr = p->get_state()->pc;
    packet->iretire = 1;
    packet->ilastsize = insn_length(insn->bits())/2;
}