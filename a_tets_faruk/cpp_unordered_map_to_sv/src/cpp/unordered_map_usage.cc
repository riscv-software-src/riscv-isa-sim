#include <stdint.h>
#include <stdio.h>
#include <unordered_map>

#include <svdpi.h>

#include "../../obj_dir_tb_assoc_arr/tb_assoc_arr__Dpi.h"

typedef uint64_t reg_t;
typedef struct
{
  uint64_t v[2];
} freg_t;

#ifndef KEY_WIDTH
#define KEY_WIDTH 64
#endif

#ifndef VALUE_WIDTH
#define VALUE_WIDTH 128
#endif

#ifndef DPI_WIDTH
#define DPI_WIDTH 32
#endif

const reg_t x1 = 1235lu, x2 = 123544lu, x3 = 1231lu, x4 = 1111lu;
const freg_t
    d1 = {{4123415lu, 0}},
    d2 = {{11lu << 32lu, 0}},
    d3 = {{0, 11lu}};

const std::unordered_map<reg_t, freg_t> map_from_c_side = {
    {x1, d1},
    {x2, d2},
    {x3, d3},
    {x4, d1},
};


/// @brief for key and value arrays: packed dimension size: dim0; num entries: dim1; entry size in packets: dim2
/// @param key_array the keys of the unordered_map is written to this array
/// @param value_array the values of the unordered_map is written to this array in same order with keys written in the key_array
/// @param num_entries_inserted is the output parameter to specify the caller how many elements are valid in the output
void write_unordered_map_to_sv_open_arrays(const svOpenArrayHandle key_array, const svOpenArrayHandle value_array, int *num_entries_inserted)
{
  printf("==============================\n");
  printf("==============================\n");
  printf("burada iki tarafin boyutlari icin asertion konulabilir\n");
  printf("key_array.entry_width: %d\n", 
    (svSize(key_array, 0) * svSize(key_array, 2)));
  printf("value_array.entry_width: %d\n", 
    (svSize(value_array, 0)*svSize(value_array,2)));

  printf("key_array.entry_count: %d\n", svSize(key_array, 1));
  printf("value_array.entry_count: %d\n", svSize(value_array, 1));
  // fflush(stdout);

#define NUM_ENTRIES (*num_entries_inserted)

  NUM_ENTRIES = 0;
  // Traversing an unordered map
  for (auto x : map_from_c_side)
  {
    // x.first // 64 bit, int32 cinsinden 2 tane
    // x.second // 128 bit, int32 cinsinden 4 tane
    // key'i part part gonder
    for (int part_ind = 0; part_ind < KEY_WIDTH / DPI_WIDTH; part_ind++)
    {
      // key_array[NUM_ENTRIES][part_ind] = *(((uint32_t *)&(x.first)) + part_ind);
      svBitVec32 temp = *(((uint32_t *)&(x.first)) + part_ind);
      svPutBitArrElemVecVal(key_array, &temp, NUM_ENTRIES, part_ind);
    }

    // value'yu part part gonder
    for (int part_ind = 0; part_ind < VALUE_WIDTH / DPI_WIDTH; part_ind++)
    {
      // value_array[NUM_ENTRIES][part_ind] = *(((uint32_t *)&(x.second)) + part_ind);
      svBitVec32 temp = *(((uint32_t *)&(x.second)) + part_ind);
      svPutBitArrElemVecVal(value_array, &temp, NUM_ENTRIES, part_ind);
    }
    NUM_ENTRIES++;
  }

#undef NUM_ENTRIES
}
