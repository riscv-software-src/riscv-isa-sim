// See LICENSE for license details.

#include "processor.h"
#include "mmu.h"
#include <cassert>

#ifdef RISCV_ENABLE_COMMITLOG
static void commit_log_reset(processor_t* p)
{
  p->get_state()->log_reg_write.clear();
  p->get_state()->log_mem_read.clear();
  p->get_state()->log_mem_write.clear();
}

static void commit_log_stash_privilege(processor_t* p)
{
  state_t* state = p->get_state();
  state->last_inst_priv = state->prv;
  state->last_inst_xlen = p->get_xlen();
  state->last_inst_flen = p->get_flen();
}

static void commit_log_print_value(FILE *log_file, int width, const void *data)
{
  assert(log_file);
  const uint64_t *arr = (const uint64_t *)data;

  fprintf(log_file, "0x");
  for (int idx = width / 64 - 1; idx >= 0; --idx) {
    fprintf(log_file, "%016" PRIx64, arr[idx]);
  }
}

static void commit_log_print_value(FILE *log_file,
                                   int width, uint64_t hi, uint64_t lo)
{
  assert(log_file);

  switch (width) {
    case 8:
      fprintf(log_file, "0x%01" PRIx8, (uint8_t)lo);
      break;
    case 16:
      fprintf(log_file, "0x%04" PRIx16, (uint16_t)lo);
      break;
    case 32:
      fprintf(log_file, "0x%08" PRIx32, (uint32_t)lo);
      break;
    case 64:
      fprintf(log_file, "0x%016" PRIx64, lo);
      break;
    case 128:
      fprintf(log_file, "0x%016" PRIx64 "%016" PRIx64, hi, lo);
      break;
    default:
      abort();
  }
}

static void commit_log_print_insn(processor_t *p, reg_t pc, insn_t insn)
{
  FILE *log_file = p->get_log_file();

  auto& reg = p->get_state()->log_reg_write;
  auto& load = p->get_state()->log_mem_read;
  auto& store = p->get_state()->log_mem_write;
  int priv = p->get_state()->last_inst_priv;
  int xlen = p->get_state()->last_inst_xlen;
  int flen = p->get_state()->last_inst_flen;

  fprintf(log_file, "%1d ", priv);
  commit_log_print_value(log_file, xlen, 0, pc);
  fprintf(log_file, " (");
  commit_log_print_value(log_file, insn.length() * 8, 0, insn.bits());
  fprintf(log_file, ")");
  bool show_vec = false;

  for (auto item : reg) {
    if (item.first == 0)
      continue;

    char prefix;
    int size;
    int rd = item.first >> 2;
    bool is_vec = false;
    bool is_vreg = false;
    switch (item.first & 3) {
    case 0:
      size = xlen;
      prefix = 'x';
      break;
    case 1:
      size = flen;
      prefix = 'f';
      break;
    case 2:
      size = p->VU.VLEN;
      prefix = 'v';
      is_vreg = true;
      break;
    case 3:
      is_vec = true;
      break;
    default:
      assert("can't been here" && 0);
      break;
    }

    if (!show_vec && (is_vreg || is_vec)) {
        if (p->VU.vflmul < 0)
          fprintf(log_file, " e%ld mf%ld l%ld", p->VU.vsew, (reg_t)(1.0/p->VU.vflmul), p->VU.vl);
        else
          fprintf(log_file, " e%ld m%ld l%ld", p->VU.vsew, (reg_t)p->VU.vflmul, p->VU.vl);
        show_vec = true;
    }

    if (!is_vec) {
      fprintf(log_file, " %c%2d ", prefix, rd);
      if (is_vreg)
        commit_log_print_value(log_file, size, &p->VU.elt<uint8_t>(rd, 0));
      else
        commit_log_print_value(log_file, size, item.second.v[1], item.second.v[0]);
    }
  }

  for (auto item : load) {
    fprintf(log_file, " mem ");
    commit_log_print_value(log_file, xlen, 0, std::get<0>(item));
  }

  for (auto item : store) {
    fprintf(log_file, " mem ");
    commit_log_print_value(log_file, xlen, 0, std::get<0>(item));
    fprintf(log_file, " ");
    commit_log_print_value(log_file, std::get<2>(item) << 3, 0, std::get<1>(item));
  }
  fprintf(log_file, "\n");
}
#else
static void commit_log_reset(processor_t* p) {}
static void commit_log_stash_privilege(processor_t* p) {}
static void commit_log_print_insn(processor_t* p, reg_t pc, insn_t insn) {}
#endif

inline void processor_t::update_histogram(reg_t pc)
{
#ifdef RISCV_ENABLE_HISTOGRAM
  pc_histogram[pc]++;
#endif
}

// This is expected to be inlined by the compiler so each use of execute_insn
// includes a duplicated body of the function to get separate fetch.func
// function calls.
static reg_t execute_insn(processor_t* p, reg_t pc, insn_fetch_t fetch)
{
  commit_log_reset(p);
  commit_log_stash_privilege(p);

  reg_t npc = fetch.func(p, fetch.insn, pc);
  if (npc != PC_SERIALIZE_BEFORE) {

#ifdef RISCV_ENABLE_COMMITLOG
    if (p->get_log_commits_enabled()) {
      commit_log_print_insn(p, pc, fetch.insn);
    }
#endif

    p->update_histogram(pc);
  }
  return npc;
}

bool processor_t::slow_path()
{
  return debug || state.single_step != state.STEP_NONE || state.debug_mode;
}

extern const char* xpr_name[NXPR];
extern const char* fpr_name[NFPR];
extern const char* vr_name[NVPR];

// fetch/decode/execute loop
void processor_t::step(size_t n)
{
  if (!state.debug_mode) {
    if (halt_request) {
      enter_debug_mode(DCSR_CAUSE_DEBUGINT);
    } // !!!The halt bit in DCSR is deprecated.
    else if (state.dcsr.halt) {
      enter_debug_mode(DCSR_CAUSE_HALT);
    }
  }

  while (n > 0) {
    size_t instret = 0;
    reg_t pc = state.pc;
    mmu_t* _mmu = mmu;

    #define advance_pc() \
     if (unlikely(invalid_pc(pc))) { \
       switch (pc) { \
         case PC_SERIALIZE_BEFORE: state.serialized = true; break; \
         case PC_SERIALIZE_AFTER: ++instret; break; \
         case PC_SERIALIZE_WFI: n = ++instret; break; \
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
      take_pending_interrupt();

      if (unlikely(slow_path()))
      {
        while (instret < n)
        {
          if (unlikely(!state.serialized && state.single_step == state.STEP_STEPPED)) {
            state.single_step = state.STEP_NONE;
            if (!state.debug_mode) {
              enter_debug_mode(DCSR_CAUSE_STEP);
              // enter_debug_mode changed state.pc, so we can't just continue.
              break;
            }
          }

          if (unlikely(state.single_step == state.STEP_STEPPING)) {
            state.single_step = state.STEP_STEPPED;
          }

#if 1
          if (debug && !prev_state) { // lazy init
            prev_reg_state_t *saved = new prev_reg_state_t;
            memcpy(&saved->VU, &VU, sizeof(vectorUnit_t));
            int v_regfile_sz = NVPR * (VU.VLEN/8);
            saved->VU.reg_file = malloc(v_regfile_sz);
            for (int i=0; i<NXPR; ++i) (reg_t&)saved->XPR[i] = 0xdeadbeefcafebabe;
            for (int i=0; i<NFPR; ++i) saved->FPR.write(i, freg(f64(161803398875)));
            for (int i=0; i<NVPR; ++i) {
              for (reg_t j=0; j<VU.VLEN/32; ++j) {
                saved->VU.elt<uint32_t>(i, j) = f32(0xdeadbeef).v;
              }
            }
            prev_state = saved;
          }
#endif
          insn_fetch_t fetch = mmu->load_insn(pc);
          if (debug && !state.serialized)
            disasm(fetch.insn);
          pc = execute_insn(this, pc, fetch);
#if 1
          if (debug && !state.serialized) {
            prev_reg_state_t *saved = prev_state;
            if (saved->VU.setvl_count != VU.setvl_count) {
              fprintf(stderr, "vconfig <- sew=%lu vlmul=%.3f vlmax=%lu vl=%lu vta=%ld vma=%ld\n",
                      VU.vsew, VU.vflmul, VU.vlmax, VU.vl, VU.vta, VU.vma);
              saved->VU.setvl_count = VU.setvl_count;
            }
            for (int i=0; i<NXPR; ++i) {
              reg_t &old = (reg_t&)saved->XPR[i];
              reg_t now = state.XPR[i];
              if (now != old) {
                fprintf(stderr, "x%d %s <- 0x%016lx %ld\n", i, xpr_name[i], now, now);
                old = now;
              }
            }
            for (int i=0; i<NFPR; ++i) {
              freg_t &old = (freg_t&)saved->FPR[i];
              freg_t now = state.FPR[i];
              if (f64(now).v != f64(old).v) {
                uint64_t v = f64(now).v;
                double dv;
                float fv;
                memcpy(&dv, &v, sizeof(dv));
                memcpy(&fv, &v, sizeof(fv));
                fprintf(stderr, "f%d %s <- 0x%016lx %f %f\n", i, fpr_name[i], v, dv, fv);
                old = now;
              }
            }
            for (reg_t i=0; i<NVPR; ++i) {
              if (!VU.reg_referenced[i]) continue;
              fprintf(stderr, "vconfig <- sew=%lu vlmul=%.3f eew=%lu emul=%.3f vlmax=%lu vl=%lu\n",
                      VU.vsew, VU.vflmul, VU.veew, VU.vemul, VU.vlmax, VU.vl);
              for (reg_t j=0; j<VU.VLEN/32; ++j) {
                uint32_t &old = saved->VU.elt<uint32_t>(i, j);
                uint32_t now = VU.elt<uint32_t>(i, j);
                if (now != old) {
                  float fv;
                  memcpy(&fv, &now, sizeof(fv));
                  fprintf(stderr, "v%ld[%3ld:%3ld] <- 0x%08x %f\n",
                          i, (j + 1) * 32 - 1, j * 32, now, fv);
                  old = now;
                }
              }
              VU.reg_referenced[i] = 0;
            }
          }
#endif
          advance_pc();
        }
      }
      else while (instret < n)
      {
        // This code uses a modified Duff's Device to improve the performance
        // of executing instructions. While typical Duff's Devices are used
        // for software pipelining, the switch statement below primarily
        // benefits from separate call points for the fetch.func function call
        // found in each execute_insn. This function call is an indirect jump
        // that depends on the current instruction. By having an indirect jump
        // dedicated for each icache entry, you improve the performance of the
        // host's next address predictor. Each case in the switch statement
        // allows for the program flow to contine to the next case if it
        // corresponds to the next instruction in the program and instret is
        // still less than n.
        //
        // According to Andrew Waterman's recollection, this optimization
        // resulted in approximately a 2x performance increase.

        // This figures out where to jump to in the switch statement
        size_t idx = _mmu->icache_index(pc);

        // This gets the cached decoded instruction from the MMU. If the MMU
        // does not have the current pc cached, it will refill the MMU and
        // return the correct entry. ic_entry->data.func is the C++ function
        // corresponding to the instruction.
        auto ic_entry = _mmu->access_icache(pc);

        // This macro is included in "icache.h" included within the switch
        // statement below. The indirect jump corresponding to the instruction
        // is located within the execute_insn() function call.
        #define ICACHE_ACCESS(i) { \
          insn_fetch_t fetch = ic_entry->data; \
          pc = execute_insn(this, pc, fetch); \
          ic_entry = ic_entry->next; \
          if (i == mmu_t::ICACHE_ENTRIES-1) break; \
          if (unlikely(ic_entry->tag != pc)) break; \
          if (unlikely(instret+1 == n)) break; \
          instret++; \
          state.pc = pc; \
        }

        // This switch statement implements the modified Duff's device as
        // explained above.
        switch (idx) {
          // "icache.h" is generated by the gen_icache script
          #include "icache.h"
        }

        advance_pc();
      }
    }
    catch(trap_t& t)
    {
      take_trap(t, pc);
      n = instret;

      if (unlikely(state.single_step == state.STEP_STEPPED)) {
        state.single_step = state.STEP_NONE;
        enter_debug_mode(DCSR_CAUSE_STEP);
      }
    }
    catch (trigger_matched_t& t)
    {
      if (mmu->matched_trigger) {
        // This exception came from the MMU. That means the instruction hasn't
        // fully executed yet. We start it again, but this time it won't throw
        // an exception because matched_trigger is already set. (All memory
        // instructions are idempotent so restarting is safe.)

        insn_fetch_t fetch = mmu->load_insn(pc);
        pc = execute_insn(this, pc, fetch);
        advance_pc();

        delete mmu->matched_trigger;
        mmu->matched_trigger = NULL;
      }
      switch (state.mcontrol[t.index].action) {
        case ACTION_DEBUG_MODE:
          enter_debug_mode(DCSR_CAUSE_HWBP);
          break;
        case ACTION_DEBUG_EXCEPTION: {
          mem_trap_t trap(CAUSE_BREAKPOINT, t.address);
          take_trap(trap, pc);
          break;
        }
        default:
          abort();
      }
    }
    catch (wait_for_interrupt_t &t)
    {
      // Return to the outer simulation loop, which gives other devices/harts a
      // chance to generate interrupts.
      //
      // In the debug ROM this prevents us from wasting time looping, but also
      // allows us to switch to other threads only once per idle loop in case
      // there is activity.
      n = instret;
    }

    state.minstret += instret;
    n -= instret;
  }
}
