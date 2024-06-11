#include "decode.h"
#include "common.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdio>

struct opcode {
  insn_bits_t match;
  insn_bits_t mask;
  std::string name;
};

static bool overlaps(const opcode& a, const opcode& b)
{
  return (a.mask & b.mask & (a.match ^ b.match)) == 0;
}

int main()
{
  #define DECLARE_INSN(name, match, mask) \
    const insn_bits_t UNUSED name##_match = (match), name##_mask = (mask);
    #include "encoding.h"
  #undef DECLARE_INSN

  static const opcode static_list[] = {
    #define DEFINE_INSN(name) \
      {name##_match, name##_mask, #name},
      #include "insn_list.h"
    #undef DEFINE_INSN
  };

  std::unordered_map<std::string, bool> overlap_list;
  #define DECLARE_OVERLAP_INSN(name, ext) \
    overlap_list[std::string(#name)] = false;
    #include "overlap_list.h"
  #undef DECLARE_OVERLAP_INSN

  std::vector<const opcode*> list;
  for (size_t i = 0; i < std::size(static_list); i++) {
    if (!overlap_list.count(static_list[i].name))
      list.push_back(&static_list[i]);
  }

  bool ok = true;

  for (size_t i = 1; i < list.size(); i++) {
    for (size_t j = 0; j < i; j++) {
      if (overlaps(*list[i], *list[j])) {
        fprintf(stderr, "Instruction %s (%" PRIx64 ") overlaps instruction %s (%" PRIx64 ", mask %" PRIx64 ")\n",
                list[i]->name.c_str(), list[i]->match, list[j]->name.c_str(), list[j]->match, list[j]->mask);
        ok = false;
      }
    }
  }

  // make sure nothing in the overlap list is unused
  for (size_t i = 1; i < std::size(static_list); i++) {
    for (size_t j = 0; j < i; j++) {
      if (overlaps(static_list[i], static_list[j])) {
        overlap_list[static_list[i].name] = true;
        overlap_list[static_list[j].name] = true;
      }
    }
  }

  for (auto const& [name, used] : overlap_list) {
    if (!used) {
      fprintf(stderr, "Instruction %s overlaps nothing, so overlap list entry has no effect\n",
              name.c_str());
      ok = false;
    }
  }

  return ok ? 0 : -1;
}
