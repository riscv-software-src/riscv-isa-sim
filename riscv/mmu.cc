#include "mmu.h"

void mmu_t::flush_tlb()
{
  memset(tlb_data, 0, sizeof(tlb_data)); // TLB entry itself has valid bit
  flush_icache();
}

void mmu_t::flush_icache()
{
  memset(icache_tag, 0, sizeof(icache_tag)); // I$ tag contains valid bit
}
