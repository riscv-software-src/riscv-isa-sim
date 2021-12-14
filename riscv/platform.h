// See LICENSE for license details.
#ifndef _RISCV_PLATFORM_H
#define _RISCV_PLATFORM_H

#define DEFAULT_RSTVEC     0x00001000
#define CLINT_BASE         0x02000000
#define CLINT_SIZE         0x000c0000
#define PLIC_BASE          0x0c000000
#define PLIC_SIZE          0x01000000
#define PLIC_NDEV          31
#define PLIC_PRIO_BITS     4
#define EXT_IO_BASE        0x40000000
#define DRAM_BASE          0x80000000

#endif
