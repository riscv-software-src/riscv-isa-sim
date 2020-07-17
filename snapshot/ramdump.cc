#include "snapshot.h"
#include <fstream>

using namespace std;

bool snapshot_t::ramdump(ofstream &out) 
{
  for (auto &mem : mems) {
    out.write((char *)&mem.first, sizeof(mem.first)); // base address
    size_t size = (*mem.second).size();
    out.write((char *)&size, sizeof(size_t));
    out.write((char *)mem.second->contents(), size);
  }
  return true;
}

std::vector<std::pair<reg_t, mem_t *>> getmem(const char *start,
                                              const char *end) 
{
  std::vector<std::pair<reg_t, mem_t *>> res;
  char *buf = (char *)start;
  while (buf < end) {
    reg_t base;
    assert(buf + sizeof(reg_t) <= end);
    memcpy((char *)&base, buf, sizeof(reg_t));
    buf += sizeof(reg_t);
    size_t size;
    assert(buf + sizeof(size_t) <= end);
    memcpy((char *)&size, buf, sizeof(size_t));
    buf += sizeof(size_t);
    mem_t *mem1 = new mem_t(size);
    char *data = mem1->contents();
    assert(buf + size <= end);
    memcpy(data, buf, size);
    buf += size;
    res.push_back(std::make_pair(base, mem1));
    assert(buf <= end);
  }
  return res;
}