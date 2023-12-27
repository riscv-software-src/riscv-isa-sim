#include <stdint.h>
#include <stdio.h>

#include <svdpi.h>

#include "../../obj_dir_tb_2/tb_2__Dpi.h"
#include "common.h"

// static size array'lere verilog tarafindan kopyalama
#define SIZE 8
data_t src1_copy[SIZE];
data_t src2_copy[SIZE];
data_t result[SIZE];

void c_init(const svOpenArrayHandle src1, const svOpenArrayHandle src2)
{
  init_out_err_files();
  int a;
  int b = scanf("%d", &a);

  for (int ii = 0; ii < SIZE; ii++)
  {
    src1_copy[ii] = svGetBitArrElem1(src1, ii);
    src2_copy[ii] = svGetBitArrElem1(src2, ii);
  }
}

void step()
{
  last_commit = result[ind] = src1_copy[ind] + src2_copy[ind];
  ind++;
}
