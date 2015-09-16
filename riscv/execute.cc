// See LICENSE for license details.

#include "processor.h"
#include "mmu.h"
#include <cassert>


static void commit_log_stash_privilege(state_t* state)
{
#ifdef RISCV_ENABLE_COMMITLOG
  state->last_inst_priv = get_field(state->mstatus, MSTATUS_PRV);
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

inline void processor_t::update_histogram(size_t pc)
{
#ifdef RISCV_ENABLE_HISTOGRAM
  size_t idx = pc >> 2;
  pc_histogram[idx]++;
#endif
}

static reg_t execute_insn(processor_t* p, reg_t pc, insn_fetch_t fetch)
{
  commit_log_stash_privilege(p->get_state());
  reg_t npc = fetch.func(p, fetch.insn, pc);
  if (npc != PC_SERIALIZE) {
    commit_log_print_insn(p->get_state(), pc, fetch.insn);
    p->update_histogram(pc);
  }
  return npc;
}

// fetch/decode/execute loop
void processor_t::step(size_t n)
{
  while (run && n > 0) {
    size_t instret = 0;
    reg_t pc = state.pc;
    mmu_t* _mmu = mmu;

    #define advance_pc() \
     if (unlikely(pc == PC_SERIALIZE)) { \
       pc = state.pc; \
       state.serialized = true; \
       break; \
     } else { \
       state.pc = pc; \
       instret++; \
     }

    try
    {
      check_timer();
      take_interrupt();

      if (unlikely(debug))
      {
        while (instret < n)
        {
          insn_fetch_t fetch = mmu->load_insn(pc);
          if (!state.serialized)
            disasm(fetch.insn);
          pc = execute_insn(this, pc, fetch);
          advance_pc();
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
    }

    state.minstret += instret;
    n -= instret;
  }
}
