#include <stdio.h>
#include <stdlib.h>

#include <svdpi.h>

#include "../../obj_dir_tb_4/tb_4__Dpi.h"
#include "common.h"

data_t *src1_glob;
data_t *src2_glob;
data_t *result_glob;

// test 4 c tarafindan verilog tarafindaki array'lere yaz
void init(svOpenArrayHandle target1, svOpenArrayHandle target2)
{
  // init_out_err_files();

  const static data_t edge_cases[][2] = {
      {0, 0},
      {0, -1},
      {-1, -1},
      {-1, 0},
  };

  int size = svSize(target1, 1);
  // print_flush_out("size: %d\n",size);
  // svLogicVecVal temp;
  // svGetLogicArrElemVecVal(&temp,target1,0);
  // printf("target1[%d]: %d\n",0,temp.aval);
  // fflush(stdout);
  int edge_case_size = sizeof(edge_cases) / (sizeof(data_t) * 2);
  int num_remaining_slots = size - edge_case_size;
  if (edge_case_size > size)
  {
    // uyari verilebilir, hata verilebilir
    edge_case_size = size;
    num_remaining_slots = 0;
  }
  if ((src1_glob = (data_t *)malloc(size * sizeof(data_t))) == NULL)
  {
  }
  if ((src2_glob = (data_t *)malloc(size * sizeof(data_t))) == NULL)
  {
  }
  if ((result_glob = (data_t *)malloc(size * sizeof(data_t))) == NULL)
  {
  }

  // array'leri doldur
  int ii;
  svLogicVecVal logic_val;
  logic_val.bval = 0;
  for (ii = 0; ii < edge_case_size; ii++)
  {
    logic_val.aval = src1_glob[ii] = edge_cases[ii][0];
    svPutLogicArrElemVecVal(target1, &logic_val, ii);
    logic_val.aval = src2_glob[ii] = edge_cases[ii][1];
    svPutLogicArrElemVecVal(target2, &logic_val, ii);
  }
  srand(0u);
  for (; ii < edge_case_size + num_remaining_slots; ii++)
  {
    logic_val.aval = src1_glob[ii] = rand() % 10;
    svPutLogicArrElemVecVal(target1, &logic_val, ii);
    logic_val.aval = src2_glob[ii] = rand() % 10;
    svPutLogicArrElemVecVal(target2, &logic_val, ii);
  }
}

void step_glob()
{
  result_glob[ind] = src1_glob[ind] + src2_glob[ind];
  last_commit = result_glob[ind];
  ind++;
}
