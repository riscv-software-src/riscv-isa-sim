// #include <cstddef>
// #include <cstdint>
// #include <cstdio>
#include "arith.h"
#include "sim.h"
#include "mmu.h"
#include "disasm.h"


bool sort_mem_region(const mem_cfg_t &a, const mem_cfg_t &b)
{
  if (a.get_base() == b.get_base())
    return (a.get_size() < b.get_size());
  else
    return (a.get_base() < b.get_base());
}

static bool check_mem_overlap(const mem_cfg_t& L, const mem_cfg_t& R)
{
  return std::max(L.get_base(), R.get_base()) <= std::min(L.get_inclusive_end(), R.get_inclusive_end());
}

static bool check_if_merge_covers_64bit_space(const mem_cfg_t& L,
                                              const mem_cfg_t& R)
{
  if (!check_mem_overlap(L, R))
    return false;

  auto start = std::min(L.get_base(), R.get_base());
  auto end = std::max(L.get_inclusive_end(), R.get_inclusive_end());

  return (start == 0ull) && (end == std::numeric_limits<uint64_t>::max());
}

static mem_cfg_t merge_mem_regions(const mem_cfg_t& L, const mem_cfg_t& R)
{
  // one can merge only intersecting regions
  assert(check_mem_overlap(L, R));

  const auto merged_base = std::min(L.get_base(), R.get_base());
  const auto merged_end_incl = std::max(L.get_inclusive_end(), R.get_inclusive_end());
  const auto merged_size = merged_end_incl - merged_base + 1;

  return mem_cfg_t(merged_base, merged_size);
}


// check the user specified memory regions and merge the overlapping or
// eliminate the containing parts
static std::vector<mem_cfg_t>
merge_overlapping_memory_regions(std::vector<mem_cfg_t> mems)
{
  if (mems.empty())
    return {};

  std::sort(mems.begin(), mems.end(), sort_mem_region);

  std::vector<mem_cfg_t> merged_mem;
  merged_mem.push_back(mems.front());

  for (auto mem_it = std::next(mems.begin()); mem_it != mems.end(); ++mem_it) {
    const auto& mem_int = *mem_it;
    if (!check_mem_overlap(merged_mem.back(), mem_int)) {
      merged_mem.push_back(mem_int);
      continue;
    }
    // there is a weird corner case preventing two memory regions from being
    // merged: if the resulting size of a region is 2^64 bytes - currently,
    // such regions are not representable by mem_cfg_t class (because the
    // actual size field is effectively a 64 bit value)
    // so we create two smaller memory regions that total for 2^64 bytes as
    // a workaround
    if (check_if_merge_covers_64bit_space(merged_mem.back(), mem_int)) {
      merged_mem.clear();
      merged_mem.push_back(mem_cfg_t(0ull, 0ull - PGSIZE));
      merged_mem.push_back(mem_cfg_t(0ull - PGSIZE, PGSIZE));
      break;
    }
    merged_mem.back() = merge_mem_regions(merged_mem.back(), mem_int);
  }

  return merged_mem;
}


std::vector<mem_cfg_t> parse_mem_layout(const char* arg)
{
  std::vector<mem_cfg_t> res;

  // handle legacy mem argument
  char* p;
  auto mb = strtoull(arg, &p, 0);
  if (*p == 0) {
    reg_t size = reg_t(mb) << 20;
    if (size != (size_t)size)
      throw std::runtime_error("Size would overflow size_t");
    res.push_back(mem_cfg_t(reg_t(DRAM_BASE), size));
    return res;
  }

  // handle base/size tuples
  while (true) {
    auto base = strtoull(arg, &p, 0);
    // if (!*p || *p != ':')
    //   help();
    auto size = strtoull(p + 1, &p, 0);

    // page-align base and size
    auto base0 = base, size0 = size;
    size += base0 % PGSIZE;
    base -= base0 % PGSIZE;
    if (size % PGSIZE != 0)
      size += PGSIZE - size % PGSIZE;

    if (size != size0) {
      fprintf(stderr, "Warning: the memory at [0x%llX, 0x%llX] has been realigned\n"
                      "to the %ld KiB page size: [0x%llX, 0x%llX]\n",
              base0, base0 + size0 - 1, long(PGSIZE / 1024), base, base + size - 1);
    }

    if (!mem_cfg_t::check_if_supported(base, size)) {
      fprintf(stderr, "Unsupported memory region "
                      "{base = 0x%llX, size = 0x%llX} specified\n",
              base, size);
      exit(EXIT_FAILURE);
    }

    const unsigned long long max_allowed_pa = (1ull << MAX_PADDR_BITS) - 1ull;
    assert(max_allowed_pa <= std::numeric_limits<reg_t>::max());
    mem_cfg_t mem_region(base, size);
    if (mem_region.get_inclusive_end() > max_allowed_pa) {
      int bits_required = 64 - clz(mem_region.get_inclusive_end());
      fprintf(stderr, "Unsupported memory region "
                      "{base = 0x%" PRIX64 ", size = 0x%" PRIX64 "} specified,"
                      " which requires %d bits of physical address\n"
                      "    The largest accessible physical address "
                      "is 0x%llX (defined by MAX_PADDR_BITS constant, which is %d)\n",
              mem_region.get_base(), mem_region.get_size(), bits_required,
              max_allowed_pa, MAX_PADDR_BITS);
      exit(EXIT_FAILURE);
    }

    res.push_back(mem_region);

    if (!*p)
      break;
    // if (*p != ',')
    //   help();
    arg = p + 1;
  }

  auto merged_mem = merge_overlapping_memory_regions(res);

  assert(!merged_mem.empty());
  return merged_mem;
}

std::vector<std::pair<reg_t, mem_t*>> make_mems(const std::vector<mem_cfg_t> &layout)
{
  std::vector<std::pair<reg_t, mem_t*>> mems;
  mems.reserve(layout.size());
  for (const auto &cfg : layout) {
    mems.push_back(std::make_pair(cfg.get_base(), new mem_t(cfg.get_size())));
  }
  return mems;
}
