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
#define CONFIG_DIFF_RVH
// #define CONFIG_DIFF_RVV  // Default off
#endif

#if defined(CPU_NUTSHELL)
#define CONFIG_DIFF_ISA_STRING "rv64imaczicsr_zifencei"
#define CONFIG_MEMORY_SIZE     (2 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x40000000UL
#define CONFIG_FLASH_SIZE      0x1000UL
#define CONFIG_PMP_NUM         0
#define CONFIG_PMP_MAX_NUM     0

#elif defined(CPU_XIANGSHAN)
#ifdef CONFIG_DIFF_RVH
    #define RVH_ISA_STRING "H"
#else
    #define RVH_ISA_STRING ""
#endif
#ifdef CONFIG_DIFF_RVV
    #define RVV_ISA_STRING "H"
#else
    #define RVV_ISA_STRING ""
#endif

#define CONFIG_DIFF_ISA_STRING \
    "RV64IMAFDC" \
    RVV_ISA_STRING \
    RVH_ISA_STRING \
    "_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zknd_zkne_zknh_zksed_zksh_svinval"

#define CONFIG_MEMORY_SIZE     (16 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x10000000UL
#define CONFIG_FLASH_SIZE      0x100000UL
#define CONFIG_PMP_NUM         16
#define CONFIG_PMP_MAX_NUM     16
#define CONFIG_PMP_GRAN        12

#elif defined(CPU_ROCKET_CHIP)
#define CONFIG_DIFF_ISA_STRING "rv64imafdczicsr_zifencei_zihpm_zicntr"
#define CONFIG_MEMORY_SIZE     (2 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x10000000UL
#define CONFIG_FLASH_SIZE      0x10000UL
#define CONFIG_PMP_NUM         0
#define CONFIG_PMP_MAX_NUM     64
#endif

#endif
