// See LICENSE for license details.
#include "decode_macros.h"
#include "v_ext_macros.h"
#include <array>
#include <cstdio>

// Independent oracle: record the EEW with which each physical register is
// read, rather than comparing register intervals as the implementation does.
static bool reserved(std::initializer_list<vector_source_t> sources)
{
  std::array<reg_t, NVPR> widths{};
  for (const auto& source : sources) {
    if (source.emul == 0)
      continue;
    const unsigned size = std::max(1, int(source.emul));
    for (unsigned r = source.reg; r < source.reg + size; ++r) {
      if (widths[r] && widths[r] != source.eew)
        return true;
      widths[r] = source.eew;
    }
  }
  return false;
}

static bool check(std::initializer_list<vector_source_t> sources)
{
  if (is_overlapped_sources(sources) == reserved(sources))
    return true;
  fprintf(stderr, "FAILED vector source overlap:");
  for (const auto& source : sources)
    fprintf(stderr, " {v%u, EMUL=%g, EEW=%u}", unsigned(source.reg),
            source.emul, unsigned(source.eew));
  fprintf(stderr, "\n");
  return false;
}

int main()
{
  unsigned tests = 0;
  for (reg_t a = 0; a < NVPR; ++a)
    for (reg_t b = 0; b < NVPR; ++b)
      for (float amul : {0.f, 0.125f, 0.25f, 0.5f, 1.f, 2.f, 4.f, 8.f})
        for (float bmul : {0.f, 0.125f, 0.25f, 0.5f, 1.f, 2.f, 4.f, 8.f}) {
          if (a + std::max(1, int(amul)) > NVPR ||
              b + std::max(1, int(bmul)) > NVPR)
            continue;
          for (reg_t aeew : {1, 8, 16, 32, 64})
            for (reg_t beew : {1, 8, 16, 32, 64}) {
              vector_source_t lhs{a, amul, aeew}, rhs{b, bmul, beew};
              // Two sources, a mask source, and an additional accumulator.
              if (!check({lhs, rhs}) || !check({rhs, lhs, {0, 1, 1}}) ||
                  !check({{0, 1, 1}, lhs, rhs, {16, 2, 64}}))
                return 1;
              tests += 3;
            }
        }
  if (!check({}))
    return 1;
  printf("Vector overlap Unit Tests: %u passed\n", tests + 1);
  return 0;
}
