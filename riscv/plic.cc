#include "devices.h"
#include "processor.h"

/* 
  Some Concepts:  
  Core: A component is termed a core if it contains an independent instruction fetch unit. 
          A RISC-V-compatible core might support multiple RISC-V-compatible hardware threads, 
          or harts, through multithreading.

  Hart: At any time, a RISC-V hardware thread (hart) is running at some privilege level 
          encoded as a mode in one or more CSRs (control and status registers).

  Context: A hart context is a privilege mode in a hardware execution thread. For example, 
             in an 4 core system with 2-way Simultaneous multithreading (SMT), you have 
             8 harts and probably at least two privilege modes per hart; machine mode 
             and supervisor mode.
*/

static reg_t pc_hook;

plic_t::plic_t(std::vector<processor_t*>& procs, size_t num_source, size_t num_context) :
  num_source(num_source), num_context(num_context), procs(procs), priority(num_source, 0), 
  ie(num_context, std::vector<plic_t::plic_reg_t>((num_source + 31) >> 5, 0)), ip((num_source + 31) >> 5),
  threshold(num_context), claimed(num_context, std::vector<plic_t::plic_reg_t>((num_source + 31) >> 5))
  {
    pc_hook == 0x80002da8;
    for (size_t i = 0; i < procs.size(); i++) {
      context_t ctx;
      ctx.mode = 'M';
      ctx.hartid = i;
      context.push_back(ctx);

      if (procs[i]->get_state()->misa | (1L << ('s' - 'a')) ) {
        ctx.mode = 'S';
        ctx.hartid = i;
        context.push_back(ctx);
      }
    }
    if (context.size() != num_context) {
      printf("[SimPLIC] Create %ld context, which should be %ld, Failed\n", context.size(), num_context);
      exit(1);
    }

    printf("[SimPLIC] Register PLIC SUCCESS with source %ld target %ld\n", num_source, num_context);
    
  }


/* 0000: Reserved (interrupt source 0 does not exist)
 * 0004 - 0FFC: Interrupt source 1 ... 1023 priority
 * 1000 - 107C: Interrupt Pending bit 0-31 ... 992-1023
 * 
 * 002000 - 00207C: Enable bits for sources 0-31 ... 992-1023 on context 0
 * 002080 - 0020FC: Enable bits for sources 0-31 ... 992-1023 on context 1
 * 002100 - 00217C: Enable bits for sources 0-31 ... 992-1023 on context 2
 * 1F1F80 - 1F1FFC: Enable bits for sources 0-31 ... 992-1023 on context 15871
 * 
 * 200000: Priority threshold for context 0
 * 200004: Claim/complete for context 0
 * 
 * 201000: Priority threshold for context 1
 * 201004: Claim/complete for context 1
 * 
 * 3FFE000: Priority threshold for context 15871
 * 3FFE004: Claim/complete for context 15871
 * 
 * 3FFFFFC: Reserved
 */

#define PLIC_PRIO_BASE       0x0
#define PLIC_IP_BASE         0x1000
#define PLIC_IE_BASE         0x2000
#define PLIC_IE_STRIDE       0x80
#define PLIC_CONTEXT_BASE    0x200000
#define PLIC_CONTEXT_STRIDE  0x1000

bool plic_t::load(reg_t addr, size_t len, uint8_t* bytes) {
  if (this->procs[0]->get_state()->pc == pc_hook) {

  }
  if (len != sizeof(plic_reg_t)) goto err;

  if (addr >= PLIC_PRIO_BASE && addr <= PLIC_PRIO_BASE + (num_source << 2)) {
    memcpy(bytes, (uint8_t*)&priority[(addr - PLIC_PRIO_BASE) >> 2], len);
  } 
  else if (addr >= PLIC_IP_BASE && addr < PLIC_IP_BASE + ((num_source + 31) >> 5)) {
    memcpy(bytes, (uint8_t*)&ip[(addr - PLIC_IP_BASE) >> 2], len);
  } 
  else if (addr >= PLIC_IE_BASE && addr < PLIC_IE_BASE + PLIC_IE_STRIDE * num_context) {
    if (((addr & (PLIC_IE_STRIDE - 1)) >> 2) < (num_source + 31) >> 5)
      memcpy(bytes, (uint8_t*)&ie[(addr - PLIC_IE_BASE) / PLIC_IE_STRIDE][(addr & (PLIC_IE_STRIDE - 1)) >> 2],len);
    else goto err;
  } 
  else if (addr >= PLIC_CONTEXT_BASE && addr < PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * num_context) {
    uint contextid = (addr - PLIC_CONTEXT_BASE) / PLIC_CONTEXT_STRIDE;
    uint offset = addr & (PLIC_CONTEXT_STRIDE - 1);
    if (offset == 0) {
      memcpy(bytes, (uint8_t*)&threshold[contextid], len);
    } 
    else if (offset == 4) {
      uint32_t value = plic_claim(contextid);
      memcpy(bytes, &value, len);
    }
    else goto err;
  } 
  else {
err:
    printf("[SimPLIC] read unknown addr %lx with %ld\n", addr, len);
    return false;
  }
  return true;
}


bool plic_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
  if (this->procs[0]->get_state()->pc == pc_hook) {
    
  }
  if (len != sizeof(plic_reg_t)) goto err;

  if (addr > PLIC_PRIO_BASE && addr <= PLIC_PRIO_BASE + (num_source << 2)) {
    memcpy((uint8_t*)&priority[(addr - PLIC_PRIO_BASE) >> 2], bytes, len);
  } 
  else if (addr >= PLIC_IP_BASE && addr < PLIC_IP_BASE + ((num_source + 31) >> 5)) {
    memcpy((uint8_t*)&ip[(addr - PLIC_IP_BASE) >> 2], bytes, len);
  }
  else if (addr >= PLIC_IE_BASE && addr < PLIC_IE_BASE + PLIC_IE_STRIDE * num_context) {
    if (((addr & (PLIC_IE_STRIDE - 1)) >> 2) < ((num_source + 31) >> 5))
      memcpy((uint8_t*)&ie[(addr - PLIC_IE_BASE) / PLIC_IE_STRIDE][(addr & (PLIC_IE_STRIDE - 1)) >> 2], bytes, len);
    else goto err;
  } 
  else if (addr >= PLIC_CONTEXT_BASE && addr < PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * num_context) {
    uint contextid = (addr - PLIC_CONTEXT_BASE) / PLIC_CONTEXT_STRIDE;
    uint offset = addr & (PLIC_CONTEXT_STRIDE - 1);

    if (offset == 0) {
        if (*(plic_reg_t*)bytes <= PLIC_MAX_PRIO) {
            memcpy((uint8_t*)&threshold[contextid], bytes, len);
        }
        else goto err;
    } 
    else if (offset == 4) {
        if (*(plic_reg_t*)bytes < num_source) {
          uint32_t irq = *(plic_reg_t*)bytes;
          claimed[contextid][irq >> 5] &= ~(1 << (irq & 31));  // clear claimed
        }
    }
    else goto err;
  } 
  else {
err:
    printf("[SimPLIC] write unknown addr %lx with %ld: %ld\n", addr, len, *(long *)bytes);
    return false;
  }
  plic_update();
  return true;
}

uint32_t plic_t::plic_claim(uint32_t contextid) {
  for (unsigned int i = 0; i < ((num_source + 31) >> 5); i++) {
      plic_reg_t pending_enabled_not_claimed = (ip[i] & ~claimed[contextid][i]) & ie[contextid][i];
      if (!pending_enabled_not_claimed) {
          continue;
      }
      for (int j = 0; j < 32; j++) {
          plic_reg_t irq = (i << 5) + j;
          plic_reg_t prio = priority[irq];
          int enabled = pending_enabled_not_claimed & (1 << j);
          if (enabled && prio > threshold[contextid]) {
              ip[i] &= ~(1 << j);                // clear pending
              claimed[contextid][i] |= 1 << j;   // set claimed
              return irq;
          }
      }
  }
  return 0;
}

bool plic_t::plic_int_check(uint32_t contextid) {
  for (unsigned int i = 0; i < ((num_source + 31) >> 5); i++) {
      plic_reg_t pending_enabled_not_claimed = (ip[i] & ~claimed[contextid][i]) & ie[contextid][i];
      if (!pending_enabled_not_claimed) {
          continue;
      }
      for (int j = 0; j < 32; j++) {
          plic_reg_t irq = (i << 5) + j;
          plic_reg_t prio = priority[irq];
          int enabled = pending_enabled_not_claimed & (1 << j);
          if (enabled && prio > threshold[contextid]) {
              return true;
          }
      }
  }
  return false;
}

void plic_t::plic_update() {
  for (size_t i = 0; i < num_context; i++) {
    bool ip = plic_int_check(i);
    switch (context[i].mode) {
      case 'M':
        if (ip)
          procs[context[i].hartid]->get_state()->mip |= MIP_MEIP;
        else
          procs[context[i].hartid]->get_state()->mip &= ~MIP_MEIP;
        break;
      case 'S':
        if (ip)
          procs[context[i].hartid]->get_state()->mip |= MIP_SEIP;
        else
          procs[context[i].hartid]->get_state()->mip &= ~MIP_SEIP;
        break;
      default: break;
    }
  }
}

void plic_t::plic_irq (uint32_t irq, bool level) {
  if (this->procs[0]->get_state()->pc == pc_hook) {
    
  }
  if (level)
    ip[irq >> 5] |= 1 << (irq & 31);      // set pending
  else
    ip[irq >> 5] &= ~(1 << (irq & 31));   // clear pending
  plic_update();
  
  // fprintf(stderr, "     prio %08x c0ie %08x ip %08x c0thr %08x c0claim %08x\n", 
  //                       priority[1], ie[0][0], ip[0],
  //                       threshold[0], claimed[0][0]);
  // fprintf(stderr, "sim: uart %d plic0 %d plic1 %d ", level, (procs[0]->get_state()->mip & MIP_MEIP) != 0, (procs[0]->get_state()->mip & MIP_SEIP) != 0);
}
