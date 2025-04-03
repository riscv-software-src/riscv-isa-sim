#include "devices.h"
#include "mmu.h"
#include <stdexcept>

mmio_device_map_t& mmio_device_map()
{
  static mmio_device_map_t device_map;
  return device_map;
}

void bus_t::add_device(reg_t addr, abstract_device_t* dev)
{
  // Allow empty devices by omitting them
  auto size = dev->size();
  if (size == 0)
    return;

  // Reject devices that overflow address size
  if (addr + size - 1 < addr) {
    fprintf(stderr, "device at [%" PRIx64 ", %" PRIx64 ") overflows address size\n",
            addr, addr + size);
    abort();
  }

  // Reject devices that overlap other devices
  if (auto it = devices.upper_bound(addr);
      (it != devices.end() && addr + size - 1 >= it->first) ||
      (it != devices.begin() && (it--, it->first + it->second->size() - 1 >= addr))) {
    fprintf(stderr, "devices at [%" PRIx64 ", %" PRIx64 ") and [%" PRIx64 ", %" PRIx64 ") overlap\n",
            it->first, it->first + it->second->size(), addr, addr + size);
    abort();
  }

  devices[addr] = dev;
}

bool bus_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  if (auto [base, dev] = find_device(addr, len); dev)
    return dev->load(addr - base, len, bytes);
  return false;
}

bool bus_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (auto [base, dev] = find_device(addr, len); dev)
    return dev->store(addr - base, len, bytes);
  return false;
}

reg_t bus_t::size()
{
  if (auto last = devices.rbegin(); last != devices.rend())
    return last->first + last->second->size();
  return 0;
}

std::pair<reg_t, abstract_device_t*> bus_t::find_device(reg_t addr)
{
  // Obtain iterator to device immediately after the one that might match
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    // No devices with base address <= addr
    return std::make_pair((reg_t)0, (abstract_device_t*)NULL);
  }

  // Rewind to device that will match if its size suffices
  it--;

  return std::make_pair(it->first, it->second);
}

std::pair<reg_t, abstract_device_t*> bus_t::find_device(reg_t addr, size_t len)
{
  if (auto [base, dev] = find_device(addr); addr - base + len - 1 < dev->size())
    return std::make_pair(base, dev);

  return std::make_pair(0, nullptr);
}

mem_t::mem_t(reg_t size)
  : sz(size)
{
  if (size == 0 || size % PGSIZE != 0)
    throw std::runtime_error("memory size must be a positive multiple of 4 KiB");
}

mem_t::~mem_t()
{
  for (auto& entry : sparse_memory_map)
    free(entry.second);
}

bool mem_t::load_store(reg_t addr, size_t len, uint8_t* bytes, bool store)
{
  if (addr + len < addr || addr + len > sz)
    return false;

  while (len > 0) {
    auto n = std::min(PGSIZE - (addr % PGSIZE), reg_t(len));

    if (store)
      memcpy(this->contents(addr), bytes, n);
    else
      memcpy(bytes, this->contents(addr), n);

    addr += n;
    bytes += n;
    len -= n;
  }

  return true;
}

char* mem_t::contents(reg_t addr) {
  reg_t ppn = addr >> PGSHIFT, pgoff = addr % PGSIZE;
  auto search = sparse_memory_map.find(ppn);
  if (search == sparse_memory_map.end()) {
    auto res = (char*)calloc(PGSIZE, 1);
    if (res == nullptr)
      throw std::bad_alloc();
    sparse_memory_map[ppn] = res;
    return res + pgoff;
  }
  return search->second + pgoff;
}

void mem_t::dump(std::ostream& o) {
  const char empty[PGSIZE] = {0};
  for (reg_t i = 0; i < sz; i += PGSIZE) {
    reg_t ppn = i >> PGSHIFT;
    auto search = sparse_memory_map.find(ppn);
    if (search == sparse_memory_map.end()) {
      o.write(empty, PGSIZE);
    } else {
      o.write(sparse_memory_map[ppn], PGSIZE);
    }
  }
}
