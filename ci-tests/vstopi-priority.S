# vstopi candidate selection (AIA 6.3.3). Run with
#   spike --isa=rv64gch_smaia_ssaia vstopi-priority
# Each case sets up CSRs in M-mode, then compares vstopi with the expected value.
# Exits through HTIF with 0 on success, else the number of the first failing case.

#define HSTATUS_VGEIN1 (1 << 12)
#define VSEI (1 << 10)
#define VTI  (1 << 30)
#define DPR  (1 << 9)
#define IPRIOM (1 << 8)
#define HVICTL(vti, iid, dpr, iprio) ((vti) | ((iid) << 16) | (dpr) | IPRIOM | (iprio))

# guest file 1 (via vsiselect/vsireg, VGEIN=1): eidelivery=1, eithreshold=0, only identity \id enabled+pending
.macro guest_irq id
  li t0, HSTATUS_VGEIN1
  csrw hstatus, t0
  li t0, 0x70
  csrw vsiselect, t0
  li t0, 1
  csrw vsireg, t0
  li t0, 0x72
  csrw vsiselect, t0
  csrw vsireg, zero
  li t0, 0xc0             # clear identities set by earlier cases
  csrw vsiselect, t0
  csrw vsireg, zero
  li t0, 0x80 + 2 * (\id / 64)
  csrw vsiselect, t0
  li t1, 1 << (\id % 64)
  csrw vsireg, t1
  li t0, 0xc0 + 2 * (\id / 64)
  csrw vsiselect, t0
  csrw vsireg, t1
.endm

.macro case num, hvictl, expected
  li s0, \num
  li t0, \hvictl
  csrw 0x609, t0          # hvictl
  csrr a0, 0xeb0          # vstopi
  li a1, \expected
  bne a0, a1, .Lfail
.endm

.macro reset
  csrw hstatus, zero
  csrw hvip, zero
  li t0, VSEI
  csrw hideleg, t0
  csrw mie, t0
.endm

.section .text.init
.globl _start
_start:
  # 1: VGEIN=1, vstopei = 0x00010001 -> IID 9, IPRIO 1 (#2557)
  reset
  guest_irq 1
  case 1, HVICTL(0, 0, 0, 0), 0x00090001

  # 2: VTI=1, IID 13 IPRIO 1 DPR=1 vs VSEI (256) -> IID 13, IPRIO 1 (#2558)
  reset
  csrw hvip, t0
  case 2, HVICTL(VTI, 13, DPR, 1), 0x000d0001

  # 3: VGEIN=1 with vstopei=5, hvictl IID 13 IPRIO 5: tie, DPR=0 -> hvictl wins
  reset
  guest_irq 5
  case 3, HVICTL(VTI, 13, 0, 5), 0x000d0005

  # 4: same tie, DPR=1 -> VSEI wins
  case 4, HVICTL(VTI, 13, DPR, 5), 0x00090005

  # 5: vstopei = 300 -> IPRIO saturates at 255
  reset
  guest_irq 300
  case 5, HVICTL(0, 0, 0, 0), 0x000900ff

  # 6: IPRIO 0, DPR=0 -> hvictl default order above VSEI -> IPRIO 0
  reset
  csrw hvip, t0
  case 6, HVICTL(VTI, 13, 0, 0), 0x000d0000

  # 7: IPRIO 0, DPR=1 -> below VSEI -> VSEI wins with 256 -> IPRIO 255
  case 7, HVICTL(VTI, 13, DPR, 0), 0x000900ff

  # 8: VGEIN=0, hvictl IID 9 IPRIO 3 -> VSEI with IPRIO 3
  case 8, HVICTL(VTI, 9, 0, 3), 0x00090003

  # 9: VGEIN=1, valid but nothing pending in the file, VSEI via hvip -> 256 -> 255
  reset
  li t0, HSTATUS_VGEIN1
  csrw hstatus, t0
  li t0, VSEI
  csrw hvip, t0
  case 9, HVICTL(0, 0, 0, 0), 0x000900ff

  li s0, 0
.Lfail:
  slli s0, s0, 1
  ori s0, s0, 1
  la t1, tohost
  sd s0, 0(t1)
1: j 1b

.section .tohost, "aw", @progbits
.align 6
.globl tohost
tohost: .dword 0
.align 6
.globl fromhost
fromhost: .dword 0
