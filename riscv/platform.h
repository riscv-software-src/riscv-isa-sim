// See LICENSE for license details.
#ifndef _RISCV_PLATFORM_H
#define _RISCV_PLATFORM_H

#define DEFAULT_KERNEL_BOOTARGS "console=ttyS0 earlycon"
#define DEFAULT_RSTVEC     0x00001000
#define DEFAULT_ISA        "rv64imafdc_zicntr_zihpm"
#define DEFAULT_PRIV       "MSU"
#define CLINT_BASE         0x02000000
#define CLINT_SIZE         0x000c0000
#define GEILEN             31
#define IMSIC_M_BASE       0x24000000
#define IMSIC_S_BASE       0x28000000
#define IMSIC_M_FILE_REGS  IMSIC_NUM_EI_REGS
#define IMSIC_S_FILE_REGS  IMSIC_NUM_EI_REGS
#define IMSIC_VS_FILE_REGS IMSIC_NUM_EI_REGS
#define PLIC_BASE          0x0c000000
#define PLIC_SIZE          0x01000000
#define PLIC_NDEV          31
#define PLIC_PRIO_BITS     4
#define APLIC_M_BASE       0x0c000000
#define APLIC_S_BASE       0x0d000000
#define APLIC_SIZE         0x00008000
#define NS16550_BASE       0x10000000
#define NS16550_SIZE       0x100
#define NS16550_REG_SHIFT  0
#define NS16550_REG_IO_WIDTH 1
#define NS16550_INTERRUPT_ID 1
#define EXT_IO_BASE        0x40000000
#define DRAM_BASE          0x80000000
#define DEBUG_START        0x0
#define DEBUG_SIZE         0x1000

#endif
