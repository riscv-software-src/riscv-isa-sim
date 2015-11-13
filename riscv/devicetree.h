// See LICENSE for license details.

#ifndef _RISCV_DEVICETREE_H
#define _RISCV_DEVICETREE_H

#include <stdint.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <arpa/inet.h>

#define FDT_MAGIC 0xd00dfeedU
#define FDT_VERSION 17
#define FDT_COMP_VERSION 16
#define FDT_BEGIN_NODE 1
#define FDT_END_NODE 2
#define FDT_PROP 3
#define FDT_END 9

struct fdt_header {
  uint32_t magic;
  uint32_t totalsize;
  uint32_t off_dt_struct;
  uint32_t off_dt_strings;
  uint32_t off_rsvmap;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

struct fdt_reserve_entry {
  uint64_t address;
  uint64_t size;
};

struct string_table {
  std::map<std::string, size_t> strings;
  std::vector<char> data;

  size_t add(std::string s) {
    if (!strings.count(s)) {
      strings[s] = data.size();
      data.insert(data.end(), s.begin(), s.end());
      data.push_back(0);
    }
    return strings[s];
  }
};

struct device_tree {
  device_tree() {
    memset(rsvmap, 0, sizeof(rsvmap));
  }

  void begin_node(std::string s) {
    std::vector<uint32_t> name = s2v(s);
    sblock.push_back(FDT_BEGIN_NODE);
    sblock.insert(sblock.end(), name.begin(), name.end());
  }

  void end_node() {
    sblock.push_back(FDT_END_NODE);
  }

  std::vector<char> finalize() {
    sblock.push_back(FDT_END);

    struct fdt_header h;
    h.size_dt_struct = sblock.size() * sizeof(sblock[0]);
    h.size_dt_strings = strings.data.size();
    h.magic = FDT_MAGIC;
    h.off_rsvmap = sizeof(h);
    h.off_dt_struct = h.off_rsvmap + sizeof(rsvmap);
    h.off_dt_strings = h.off_dt_struct + h.size_dt_struct;
    h.totalsize = h.off_dt_strings + h.size_dt_strings;
    h.version = FDT_VERSION;
    h.last_comp_version = FDT_COMP_VERSION;
    h.boot_cpuid_phys = 0;

    for (uint32_t* p = &h.magic; p < &h.magic + sizeof(h)/sizeof(uint32_t); p++)
      *p = htonl(*p);
    for (uint32_t& p : sblock)
      p = htonl(p);

    std::vector<char> res;
    res.insert(res.end(), (char*)&h, (char*)&h + sizeof(h));
    res.insert(res.end(), (char*)&rsvmap, (char*)&rsvmap + sizeof(rsvmap));
    res.insert(res.end(), (char*)&sblock[0],
               (char*)&sblock[0] + sblock.size() * sizeof(sblock[0]));
    res.insert(res.end(), strings.data.begin(), strings.data.end());
    return res;
  }
  
  void add_prop(std::string name, uint32_t data)
  {
    add_prop(name, std::vector<uint32_t>(1, data), sizeof(data));
  }
  
  void add_reg(std::vector<uint64_t> values)
  {
    std::vector<uint32_t> v;
    for (auto x : values) {
      v.push_back(x >> 32);
      v.push_back(x);
    }
    add_prop("reg", v, v.size() * sizeof(v[0]));
  }
  
  void add_prop(std::string name, std::string data)
  {
    add_prop(name, s2v(data), data.size()+1);
  }

 private:
  struct string_table strings;
  std::vector<uint32_t> sblock;
  struct fdt_reserve_entry rsvmap[1];

  std::vector<uint32_t> s2v(std::string data) {
    std::vector<char> v(data.begin(), data.end());
    do {
      v.push_back(0);
    } while (v.size() % 4);
  
    std::vector<uint32_t> words;
    for (size_t i = 0; i < v.size(); i += 4)
      words.push_back((v[i] << 24) | (v[i+1] << 16) | (v[i+2] << 8) | v[i+3]);
    return words;
  }
  
  void add_prop(std::string name, std::vector<uint32_t> data, size_t len) {
    sblock.push_back(FDT_PROP);
    sblock.push_back(len);
    sblock.push_back(strings.add(name));
    sblock.insert(sblock.end(), data.begin(), data.end());
  }
};

#endif
