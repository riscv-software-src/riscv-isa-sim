#include "decode.h"
#include "common.h"
#include <unordered_set>
#include <vector>
#include <string>
#include <cstdio>

struct opcode {
  insn_bits_t match;
  insn_bits_t mask;
  std::string name;
};

static void check_overlap(const opcode& a, const opcode& b)
{
  if ((a.match & b.mask) == b.match) {
    fprintf(stderr, "Instruction %s (%" PRIx64 ") overlaps instruction %s (%" PRIx64 ", mask %" PRIx64 ")\n",
            a.name.c_str(), a.match, b.name.c_str(), b.match, b.mask);
    exit(-1);
  }
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

  std::unordered_set<std::string> overlap_list;
  #define DECLARE_OVERLAP_INSN(name, ext) \
    overlap_list.insert(std::string(#name));
    #include "overlap_list.h"
  #undef DECLARE_OVERLAP_INSN

  std::vector<const opcode*> list;
  for (size_t i = 0; i < sizeof(static_list) / sizeof(static_list[0]); i++) {
    if (!overlap_list.count(static_list[i].name))
      list.push_back(&static_list[i]);
  }

  for (size_t i = 1; i < list.size(); i++) {
    for (size_t j = 0; j < i; j++) {
      check_overlap(*list[i], *list[j]);
      check_overlap(*list[j], *list[i]);
    }
  }

  return 0;
}
