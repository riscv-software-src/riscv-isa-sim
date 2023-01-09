// See LICENSE for license details.

#include "cfg.h"
#include "mmu.h"
#include "decode.h"

mem_cfg_t::mem_cfg_t(reg_t base, reg_t size) : base(base), size(size)
{
  assert(mem_cfg_t::check_if_supported(base, size));
}

bool mem_cfg_t::check_if_supported(reg_t base, reg_t size)
{
  // The truth of these conditions should be ensured by whatever is creating
  // the regions in the first place, but we have them here to make sure that
  // we can't end up describing memory regions that don't make sense. They
  // ask that the page size is a multiple of the minimum page size, that the
  // page is aligned to the minimum page size, that the page is non-empty and
  // that the top address is still representable in a reg_t.
  //
  // Note: (base + size == 0) part of the assertion is to handle cases like
  //   { base = 0xffff_ffff_ffff_f000, size: 0x1000 }
  return (size % PGSIZE == 0) &&
         (base % PGSIZE == 0) &&
         (size > 0) &&
         ((base + size > base) || (base + size == 0));
}
