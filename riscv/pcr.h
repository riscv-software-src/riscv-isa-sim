// See LICENSE for license details.

#ifndef _RISCV_PCR_H
#define _RISCV_PCR_H

#define SR_S     0x00000001
#define SR_PS    0x00000002
#define SR_EI    0x00000004
#define SR_PEI   0x00000008
#define SR_EF    0x00000010
#define SR_U64   0x00000020
#define SR_S64   0x00000040
#define SR_VM    0x00000080
#define SR_EV    0x00000100
#define SR_IM    0x00FF0000
#define SR_IP    0xFF000000
#define SR_ZERO  ~(SR_S|SR_PS|SR_EI|SR_PEI|SR_EF|SR_U64|SR_S64|SR_VM|SR_EV|SR_IM|SR_IP)
#define SR_IM_SHIFT 16
#define SR_IP_SHIFT 24

#define PCR_SR       0
#define PCR_EPC      1
#define PCR_BADVADDR 2
#define PCR_EVEC     3
#define PCR_CAUSE    4
#define PCR_PTBR     5
#define PCR_ASID     6
#define PCR_FATC     7
#define PCR_COUNT    8
#define PCR_COMPARE  9
#define PCR_SEND_IPI 10
#define PCR_CLR_IPI  11
#define PCR_HARTID   12
#define PCR_IMPL     13
#define PCR_K0       14
#define PCR_K1       15
#define PCR_VECBANK  18
#define PCR_VECCFG   19
#define PCR_RESET    29
#define PCR_TOHOST   30
#define PCR_FROMHOST 31

#define IRQ_IPI   5
#define IRQ_HOST  6
#define IRQ_TIMER 7

#define CAUSE_MISALIGNED_FETCH 0
#define CAUSE_FAULT_FETCH 1
#define CAUSE_ILLEGAL_INSTRUCTION 2
#define CAUSE_PRIVILEGED_INSTRUCTION 3
#define CAUSE_FP_DISABLED 4
#define CAUSE_SYSCALL 6
#define CAUSE_BREAKPOINT 7
#define CAUSE_MISALIGNED_LOAD 8
#define CAUSE_MISALIGNED_STORE 9
#define CAUSE_FAULT_LOAD 10
#define CAUSE_FAULT_STORE 11
#define CAUSE_VECTOR_DISABLED 12
#define CAUSE_VECTOR_BANK 13

#define CAUSE_VECTOR_MISALIGNED_FETCH 24
#define CAUSE_VECTOR_FAULT_FETCH 25
#define CAUSE_VECTOR_ILLEGAL_INSTRUCTION 26
#define CAUSE_VECTOR_ILLEGAL_COMMAND 27
#define CAUSE_VECTOR_MISALIGNED_LOAD 28
#define CAUSE_VECTOR_MISALIGNED_STORE 29
#define CAUSE_VECTOR_FAULT_LOAD 30
#define CAUSE_VECTOR_FAULT_STORE 31

// page table entry (PTE) fields
#define PTE_V    0x001 // Entry is a page Table descriptor
#define PTE_T    0x002 // Entry is a page Table, not a terminal node
#define PTE_G    0x004 // Global
#define PTE_UR   0x008 // User Write permission
#define PTE_UW   0x010 // User Read permission
#define PTE_UX   0x020 // User eXecute permission
#define PTE_SR   0x040 // Supervisor Read permission
#define PTE_SW   0x080 // Supervisor Write permission
#define PTE_SX   0x100 // Supervisor eXecute permission
#define PTE_PERM (PTE_SR | PTE_SW | PTE_SX | PTE_UR | PTE_UW | PTE_UX)

#ifdef __riscv

#ifdef __riscv64
# define RISCV_PGLEVELS 3
# define RISCV_PGSHIFT 13
#else
# define RISCV_PGLEVELS 2
# define RISCV_PGSHIFT 12
#endif
#define RISCV_PGLEVEL_BITS 10
#define RISCV_PGSIZE (1 << RISCV_PGSHIFT)

#define ASM_CR(r)   _ASM_CR(r)
#define _ASM_CR(r)  cr##r

#ifndef __ASSEMBLER__

#define mtpcr(reg,val) ({ long __tmp = (long)(val), __tmp2; \
          asm volatile ("mtpcr %0,%1,cr%2" : "=r"(__tmp2) : "r"(__tmp),"i"(reg)); \
          __tmp2; })

#define mfpcr(reg) ({ long __tmp; \
          asm volatile ("mfpcr %0,cr%1" : "=r"(__tmp) : "i"(reg)); \
          __tmp; })

#define setpcr(reg,val) ({ long __tmp; \
          asm volatile ("setpcr %0,cr%2,%1" : "=r"(__tmp) : "i"(val), "i"(reg)); \
          __tmp; })

#define clearpcr(reg,val) ({ long __tmp; \
          asm volatile ("clearpcr %0,cr%2,%1" : "=r"(__tmp) : "i"(val), "i"(reg)); \
          __tmp; })

#define rdcycle() ({ unsigned long __tmp; \
          asm volatile ("rdcycle %0" : "=r"(__tmp)); \
          __tmp; })

#endif

#endif

#endif
