// See LICENSE for license details.
#include <cinttypes>
#include <cstdio>
#include <cstdlib>

#include <riscv/vector_unit.h>

struct test_case {
  reg_t te, tew, tile, row, col, offset;
};

// Starting byte offsets in the linearized tile state, from the Zvt v0.4
// Tile Punning Aliasing mapping. Cover TE=4 (where the previous mapping
// agrees), both sides of row partition boundaries, and the highest tiles.
static const test_case cases[] = {
  {  4, 32,  0,   0,   0,      0},
  {  4, 32,  0,   1,   3,     28},
  {  4, 32, 12,   3,   3,    252},
  {  4, 64,  0,   0,   1,      8},
  {  4, 64, 14,   1,   1,    248},
  {  8, 32,  0,   2,   0,     32},
  {  8, 32,  4,   3,   3,    364},
  {  8, 32, 12,   4,   4,    912},
  {  8, 32, 12,   7,   7,   1020},
  {  8, 64,  0,   1,   0,     32},
  {  8, 64,  6,   2,   2,    464},
  {  8, 64, 14,   3,   3,   1016},
  { 16, 32,  0,   2,   0,     64},
  { 16, 32,  4,   7,   2,   1480},
  { 16, 32, 12,   8,   4,   3600},
  { 16, 32, 12,  15,  15,   4092},
  { 16, 64,  0,   1,   0,     64},
  { 16, 64,  6,   4,   2,   1808},
  { 16, 64, 14,   7,   7,   4088},
  { 32, 32,  0,   2,   0,    128},
  { 32, 32,  4,  15,   3,   6028},
  { 32, 32, 12,  16,   4,  14352},
  { 32, 32, 12,  31,  31,  16380},
  { 32, 64,  0,   1,   0,    128},
  { 32, 64,  6,   8,   2,   7184},
  { 32, 64, 14,  15,  15,  16376},
  { 64, 32, 12,  32,   4,  57360},
  { 64, 32, 12,  63,  63,  65532},
  { 64, 64, 14,  16,   2,  61456},
  { 64, 64, 14,  31,  31,  65528},
  {128, 32, 12,  64,   4, 229392},
  {128, 32, 12, 127, 127, 262140},
  {128, 64, 14,  32,   2, 245776},
  {128, 64, 14,  63,  63, 262136},
};

template<typename T>
static bool check(vectorUnit_t &vu, const test_case &c) {
  reg_t offset = reinterpret_cast<char *>(&vu.mt_elt<T>(c.tile, c.row, c.col))
                 - vu.mt.data();
  if (offset == c.offset)
    return true;

  fprintf(stderr, "FAIL: TE=%" PRIu64 ", TEW=%" PRIu64 ", tile=%" PRIu64
          ", row=%" PRIu64 ", col=%" PRIu64 ": offset=%" PRIu64
          ", expected=%" PRIu64 "\n",
          c.te, c.tew, c.tile, c.row, c.col, offset, c.offset);
  return false;
}

int main() {
  vectorUnit_t vu;
  bool ok = true;
  for (const auto &c : cases) {
    vu.TE = c.te;
    vu.mt.resize(16 * c.te * c.te);
    ok &= c.tew == 32 ? check<uint32_t>(vu, c) : check<uint64_t>(vu, c);
  }

  if (!ok)
    return EXIT_FAILURE;
  printf("Executed successfully\n");
  return EXIT_SUCCESS;
}
