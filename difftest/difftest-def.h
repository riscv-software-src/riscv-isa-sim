#ifndef __DIFFTEST_DEF_H
#define __DIFFTEST_DEF_H

#if defined(CPU_NUTSHELL)
#elif defined(CPU_XIANGSHAN)
#elif defined(CPU_ROCKET_CHIP)
#else
// This is the default CPU
#define CPU_NUTSHELL
#endif

#if defined(CPU_XIANGSHAN) || defined(CPU_ROCKET_CHIP)
#define CONFIG_DIFF_FPU
#endif

#if defined(CPU_XIANGSHAN)
#define CONFIG_DIFF_DEBUG_MODE
#endif

#if defined(CPU_NUTSHELL)
#define CONFIG_DIFF_ISA_STRING "rv64imaczicsr_zifencei"
#define CONFIG_MEMORY_SIZE     (2 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x40000000UL
#define CONFIG_FLASH_SIZE      0x1000UL
#elif defined(CPU_XIANGSHAN)
#define CONFIG_DIFF_ISA_STRING "RV64IMAFDC_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zknd_zkne_zknh_zksed_zksh_svinval"
#define CONFIG_MEMORY_SIZE     (16 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x10000000UL
#define CONFIG_FLASH_SIZE      0x100000UL
#elif defined(CPU_ROCKET_CHIP)
#define CONFIG_DIFF_ISA_STRING "rv64imafdczicsr_zifencei_zihpm_zicntr"
#define CONFIG_MEMORY_SIZE     (2 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x10000000UL
#define CONFIG_FLASH_SIZE      0x10000UL
#endif

#endif
