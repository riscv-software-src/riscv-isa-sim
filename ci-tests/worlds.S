// Test of the RISC-V Worlds (World-ID) extensions:
// Smwid, Smlwid, Smlwidlist, Smwiddeleg and Sswid.
//
// Run with:
//   spike --isa=rv64gc_smwid_smlwid_smlwidlist_smwiddeleg_sswid --priv=msu \
//         --nworlds=4 --pmlwidlist=0x7 worlds
//
// i.e. NWorlds=4 (a 2-bit WID field), pmwid=0, mwid unlocked out of
// reset, all WIDs permitted in mwid, WIDs 0-2 permitted in mlwid and all
// permitted WIDs delegatable to S-mode.

#define CSR_SLWID     0x190
#define CSR_MLWID     0x390
#define CSR_MWIDDELEG 0x748
#define CSR_MWID      0x749
#define CSR_MLWIDLIST 0x74a

#define MSTATUS_MPP   0x1800
#define MSTATUS_MPP_S 0x0800
#define MSTATUS_MPRV  0x20000
#define SSTATUS_SPP   0x100
#define MWID64_L      0x8000000000000000

#define CAUSE_ILLEGAL_INSTRUCTION 2
#define CAUSE_USER_ECALL          8
#define CAUSE_SUPERVISOR_ECALL    9
#define CAUSE_SOFTWARE_CHECK      18
#define ILLEGAL_WORLD_FAULT       4

// Register conventions used by the trap handler:
//   s2: expected mcause
//   s3: expected mtval, or -1 to not check mtval
//   s4: set to 1 when the expected trap has been taken
//   s5: address to resume at (in M-mode) after the trap, or 0 to resume
//       at the instruction after the trapping one

#define EXPECT_TRAP(cause, tval, resume) \
  li s2, cause;                          \
  li s3, tval;                           \
  li s4, 0;                              \
  resume

#define RESUME_NEXT li s5, 0
#define RESUME_AT(label) la s5, label

#define CHECK_CSR(csr, expected) \
  csrr t0, csr;                  \
  li t1, expected;               \
  bne t0, t1, fail

#define CHECK_TRAP_TAKEN \
  li t1, 1;              \
  bne s4, t1, fail

.section .text.init
.globl _start
_start:
  la t0, mtrap
  csrw mtvec, t0

  // 1. mwid resets to pmwid (0) and is unlocked.  Only the 2-bit WID field
  //    and the lock bit are writable; the reserved bits read as zero.
  CHECK_CSR(CSR_MWID, 0)
  li t0, 0xFF3
  csrw CSR_MWID, t0
  CHECK_CSR(CSR_MWID, 3)
  csrw CSR_MWID, x0
  CHECK_CSR(CSR_MWID, 0)

  // 2. mlwid: only the 2-bit WID field is writable.
  CHECK_CSR(CSR_MLWID, 0)
  li t0, 0xFF2
  csrw CSR_MLWID, t0
  CHECK_CSR(CSR_MLWID, 2)

  // 3. mlwidlist resets to pmlwidlist (0x7) and is WARL-masked by it.
  CHECK_CSR(CSR_MLWIDLIST, 0x7)
  li t0, 0xF
  csrw CSR_MLWIDLIST, t0
  CHECK_CSR(CSR_MLWIDLIST, 0x7)

  // 4. mwiddeleg resets to zero, so Sswid is disabled and slwid accesses
  //    raise an illegal instruction exception (even from M-mode).
  CHECK_CSR(CSR_MWIDDELEG, 0)
  EXPECT_TRAP(CAUSE_ILLEGAL_INSTRUCTION, -1, RESUME_NEXT)
  csrr t0, CSR_SLWID
  CHECK_TRAP_TAKEN

  // 5. mwiddeleg is a WARL subset of mlwidlist/pmlwidlist, and clearing
  //    bits in mlwidlist clears them in mwiddeleg.
  li t0, 0xF
  csrw CSR_MWIDDELEG, t0
  CHECK_CSR(CSR_MWIDDELEG, 0x7)
  li t0, 0x3
  csrw CSR_MLWIDLIST, t0
  CHECK_CSR(CSR_MLWIDLIST, 0x3)
  CHECK_CSR(CSR_MWIDDELEG, 0x3)
  li t0, 0x7
  csrw CSR_MLWIDLIST, t0
  CHECK_CSR(CSR_MLWIDLIST, 0x7)
  CHECK_CSR(CSR_MWIDDELEG, 0x3)
  csrw CSR_MWIDDELEG, x0
  CHECK_CSR(CSR_MWIDDELEG, 0)

  // 6. S-mode runs normally with an authorized mlwid (2).
  li t0, 2
  csrw CSR_MLWID, t0
  EXPECT_TRAP(CAUSE_SUPERVISOR_ECALL, -1, RESUME_AT(after_s_ecall))
  la t0, s_ecall
  jal enter_s_mode
after_s_ecall:
  CHECK_TRAP_TAKEN

  // 7. With an unauthorized mlwid (3 is not in pmlwidlist), the first
  //    instruction fetch in S-mode raises a software-check exception with
  //    mtval = illegal world fault.
  li t0, 3
  csrw CSR_MLWID, t0
  EXPECT_TRAP(CAUSE_SOFTWARE_CHECK, ILLEGAL_WORLD_FAULT, RESUME_AT(after_s_fault))
  la t0, s_ecall
  jal enter_s_mode
after_s_fault:
  CHECK_TRAP_TAKEN

  // 8. Likewise for an M-mode load performed with MPRV=1 and MPP=S.
  li t0, MSTATUS_MPP
  csrc mstatus, t0
  li t0, MSTATUS_MPP_S | MSTATUS_MPRV
  csrs mstatus, t0
  EXPECT_TRAP(CAUSE_SOFTWARE_CHECK, ILLEGAL_WORLD_FAULT, RESUME_NEXT)
  la t0, scratch
  ld t1, 0(t0)
  CHECK_TRAP_TAKEN
  li t0, MSTATUS_MPRV
  csrc mstatus, t0

  // 9. With an authorized mlwid (0) and an unauthorized mlwidlist-restricted
  //    value, mlwidlist takes effect: restrict to {0, 1} and use mlwid = 2.
  li t0, 0x3
  csrw CSR_MLWIDLIST, t0
  li t0, 2
  csrw CSR_MLWID, t0
  EXPECT_TRAP(CAUSE_SOFTWARE_CHECK, ILLEGAL_WORLD_FAULT, RESUME_AT(after_mlwidlist_fault))
  la t0, s_ecall
  jal enter_s_mode
after_mlwidlist_fault:
  CHECK_TRAP_TAKEN
  li t0, 0x7
  csrw CSR_MLWIDLIST, t0

  // 10. Sswid: delegate WIDs {0, 1} to S-mode.  S-mode writes an
  //     unauthorized slwid (2) and enters U-mode, whose first fetch raises a
  //     software-check exception.
  li t0, 1
  csrw CSR_MLWID, t0
  li t0, 0x3
  csrw CSR_MWIDDELEG, t0
  CHECK_CSR(CSR_MWIDDELEG, 0x3)
  CHECK_CSR(CSR_SLWID, 0)
  EXPECT_TRAP(CAUSE_SOFTWARE_CHECK, ILLEGAL_WORLD_FAULT, RESUME_AT(after_u_fault))
  la t0, s_set_bad_slwid_enter_u
  jal enter_s_mode
after_u_fault:
  CHECK_TRAP_TAKEN
  CHECK_CSR(CSR_SLWID, 2)

  // 11. With an authorized slwid (1), U-mode runs normally.
  li t0, 1
  csrw CSR_SLWID, t0
  EXPECT_TRAP(CAUSE_USER_ECALL, -1, RESUME_AT(after_u_ecall))
  la t0, s_enter_u
  jal enter_s_mode
after_u_ecall:
  CHECK_TRAP_TAKEN

  // 12. Likewise for an M-mode load performed with MPRV=1 and MPP=U and an
  //     unauthorized slwid.
  li t0, 2
  csrw CSR_SLWID, t0
  li t0, MSTATUS_MPP
  csrc mstatus, t0
  li t0, MSTATUS_MPRV
  csrs mstatus, t0
  EXPECT_TRAP(CAUSE_SOFTWARE_CHECK, ILLEGAL_WORLD_FAULT, RESUME_NEXT)
  la t0, scratch
  ld t1, 0(t0)
  CHECK_TRAP_TAKEN
  li t0, MSTATUS_MPRV
  csrc mstatus, t0

  // 13. Clearing mwiddeleg disables Sswid again: slwid is illegal in S-mode.
  csrw CSR_MWIDDELEG, x0
  EXPECT_TRAP(CAUSE_ILLEGAL_INSTRUCTION, -1, RESUME_AT(after_s_illegal))
  la t0, s_read_slwid
  jal enter_s_mode
after_s_illegal:
  CHECK_TRAP_TAKEN

  // 14. Locking mwid makes mwid and mlwidlist read-only.
  li t0, MWID64_L
  csrw CSR_MWID, t0
  li t1, MWID64_L
  csrr t0, CSR_MWID
  bne t0, t1, fail
  li t0, 1
  csrw CSR_MWID, t0
  csrr t0, CSR_MWID
  bne t0, t1, fail
  li t0, 0x1
  csrw CSR_MLWIDLIST, t0
  CHECK_CSR(CSR_MLWIDLIST, 0x7)
  // mlwid and mwiddeleg are unaffected by the lock.
  li t0, 0
  csrw CSR_MLWID, t0
  CHECK_CSR(CSR_MLWID, 0)
  li t0, 0x1
  csrw CSR_MWIDDELEG, t0
  CHECK_CSR(CSR_MWIDDELEG, 0x1)

  li t1, 1
  j exit

fail:
  li t1, 3

exit:
  lla t0, tohost
  sd t1, 0(t0)
1:
  j 1b

// Enter S-mode at the address in t0, with sstatus.SPP = 0 so that an sret
// enters U-mode.
enter_s_mode:
  csrw mepc, t0
  li t0, MSTATUS_MPP
  csrc mstatus, t0
  li t0, MSTATUS_MPP_S
  csrs mstatus, t0
  li t0, SSTATUS_SPP
  csrc sstatus, t0
  mret

.align 2
s_ecall:
  ecall
  j fail

.align 2
s_set_bad_slwid_enter_u:
  li t0, 2
  csrw CSR_SLWID, t0
s_enter_u:
  la t0, u_ecall
  csrw sepc, t0
  sret

.align 2
s_read_slwid:
  csrr t0, CSR_SLWID
  j fail

.align 2
u_ecall:
  ecall
  j fail

.align 2
mtrap:
  csrr t5, mcause
  bne t5, s2, fail
  li t6, -1
  beq s3, t6, 1f
  csrr t5, mtval
  bne t5, s3, fail
1:
  li s4, 1
  beqz s5, 2f
  csrw mepc, s5
  j 3f
2:
  csrr t5, mepc
  addi t5, t5, 4
  csrw mepc, t5
3:
  // Always return to M-mode.
  li t5, MSTATUS_MPP
  csrs mstatus, t5
  mret

.section .data
.balign 8
scratch:
  .dword 0

.section .tohost,"aw",@progbits
.balign 64
.globl tohost
tohost:
  .dword 0
.balign 64
.globl fromhost
fromhost:
  .dword 0
