// See LICENSE for license details.

#include "processor.h"
#include "mmu.h"
#include "sim.h"
#include <cassert>


static void commit_log_stash_privilege(state_t* state)
{
#ifdef RISCV_ENABLE_COMMITLOG
  state->last_inst_priv = state->prv;
#endif
}

static void commit_log_print_insn(state_t* state, reg_t pc, insn_t insn)
{
#ifdef RISCV_ENABLE_COMMITLOG
  int32_t priv = state->last_inst_priv;
  uint64_t mask = (insn.length() == 8 ? uint64_t(0) : (uint64_t(1) << (insn.length() * 8))) - 1;
  if (state->log_reg_write.addr) {
    fprintf(stderr, "%1d 0x%016" PRIx64 " (0x%08" PRIx64 ") %c%2" PRIu64 " 0x%016" PRIx64 "\n",
            priv,
            pc,
            insn.bits() & mask,
            state->log_reg_write.addr & 1 ? 'f' : 'x',
            state->log_reg_write.addr >> 1,
            state->log_reg_write.data);
  } else {
    fprintf(stderr, "%1d 0x%016" PRIx64 " (0x%08" PRIx64 ")\n", priv, pc, insn.bits() & mask);
  }
  state->log_reg_write.addr = 0;
#endif
}

inline void processor_t::update_histogram(reg_t pc)
{
#ifdef RISCV_ENABLE_HISTOGRAM
  pc_histogram[pc]++;
#endif
}

static reg_t execute_insn(processor_t* p, reg_t pc, insn_fetch_t fetch)
{
  commit_log_stash_privilege(p->get_state());
  reg_t npc = fetch.func(p, fetch.insn, pc);
  if (!invalid_pc(npc)) {
    commit_log_print_insn(p->get_state(), pc, fetch.insn);
    p->update_histogram(pc);
  }
  return npc;
}

// fetch/decode/execute loop
void processor_t::step(size_t n)
{
  if (state.dcsr.cause == DCSR_CAUSE_NONE) {
    // TODO: get_interrupt() isn't super fast. Does that matter?
    if (sim->debug_module.get_interrupt(id)) {
      enter_debug_mode(DCSR_CAUSE_DEBUGINT);
    } else if (state.dcsr.halt) {
      enter_debug_mode(DCSR_CAUSE_HALT);
    }
  } else {
    // In Debug Mode, just do 11 steps at a time. Otherwise we're going to be
    // spinning the rest of the time anyway.
    n = std::min(n, (size_t) 11);
  }

  while (n > 0) {
    size_t instret = 0;
    reg_t pc = state.pc;
    mmu_t* _mmu = mmu;

    #define advance_pc() \
     if (unlikely(invalid_pc(pc))) { \
       switch (pc) { \
         case PC_SERIALIZE_BEFORE: state.serialized = true; break; \
         case PC_SERIALIZE_AFTER: instret++; break; \
         default: abort(); \
       } \
       pc = state.pc; \
       break; \
     } else { \
       state.pc = pc; \
       instret++; \
     }

    try
    {
      take_interrupt();

      // When we might single step, use the slow loop instead of the fast one.
      if (unlikely(debug || state.single_step != state.STEP_NONE || state.dcsr.cause))
      {
        while (instret < n)
        {
          if (unlikely(state.single_step == state.STEP_STEPPING)) {
            state.single_step = state.STEP_STEPPED;
          }

          insn_fetch_t fetch = mmu->load_insn(pc);
          if (debug && !state.serialized)
            disasm(fetch.insn);
          pc = execute_insn(this, pc, fetch);
          bool serialize_before = (pc == PC_SERIALIZE_BEFORE);

          advance_pc();

          if (unlikely(state.single_step == state.STEP_STEPPED) && !serialize_before) {
            state.single_step = state.STEP_NONE;
            enter_debug_mode(DCSR_CAUSE_STEP);
            // enter_debug_mode changed state.pc, so we can't just continue.
            break;
          }
        }
      }
      else while (instret < n)
      {
        size_t idx = _mmu->icache_index(pc);
        auto ic_entry = _mmu->access_icache(pc);

        #define ICACHE_ACCESS(i) { \
          insn_fetch_t fetch = ic_entry->data; \
          ic_entry++; \
          pc = execute_insn(this, pc, fetch); \
          if (i == mmu_t::ICACHE_ENTRIES-1) break; \
          if (unlikely(ic_entry->tag != pc)) goto miss; \
          if (unlikely(instret+1 == n)) break; \
          instret++; \
          state.pc = pc; \
        }

        switch (idx) {
          #include "icache.h"
        }

        advance_pc();
        continue;

miss:
        advance_pc();
        // refill I$ if it looks like there wasn't a taken branch
        if (pc > (ic_entry-1)->tag && pc <= (ic_entry-1)->tag + MAX_INSN_LENGTH)
          _mmu->refill_icache(pc, ic_entry);
      }
    }
    catch(trap_t& t)
    {
      take_trap(t, pc);
      n = instret;
    }

    state.minstret += instret;
    n -= instret;
  }
}
