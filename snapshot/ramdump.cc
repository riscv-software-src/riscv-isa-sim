#include "snapshot.h"
#include <fstream>

using namespace std;

bool snapshot_t::ramdump(ofstream &out) 
{
  for (auto tag: *tags) {
    out.write((char *)&tag.first, sizeof(tag.first)); // vaddr
    size_t size = 1 << PGSHIFT;
    char *addr = sim -> addr_to_mem(tag.first * size);
    if(addr == NULL) 
      return false;
    out.write((char *)addr, size);
  }
  return true;
}

std::vector<std::pair<reg_t, char *>> getmem(const char *start,
                                              const char *end) 
{
  std::vector<std::pair<reg_t, char *>> res;
  char *buf = (char *)start;
  while (buf < end) {
    reg_t base;
    assert(buf + sizeof(reg_t) <= end);
    memcpy((char *)&base, buf, sizeof(reg_t));
    buf += sizeof(reg_t);
    size_t size = 1 << PGSHIFT;
    char *data = (char *)malloc(size);
    assert(buf + size <= end);
    memcpy(data, buf, size);
    buf += size;
    res.push_back(std::make_pair(base, data));
    assert(buf <= end);
  }
  return res;
}