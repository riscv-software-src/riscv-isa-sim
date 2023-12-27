#include <stdlib.h>
#include <stdio.h>

#include <svdpi.h>

#include "../../obj_dir_tb_3/tb_3__Dpi.h"
#include "common.h"

data_t *src1_copy_dynamic;
data_t *src2_copy_dynamic;
data_t *result_dynamic;

// test 3 c tarafinda array'leri malloc'la olustur
void c_init_dynamic(const svOpenArrayHandle src1, const svOpenArrayHandle src2)
{
  // init_out_err_files();
  int size = svSize(src1, 1);

  src1_copy_dynamic = (data_t *)malloc(size * sizeof(data_t));
  src2_copy_dynamic = (data_t *)malloc(size * sizeof(data_t));
  result_dynamic = (data_t *)malloc(size * sizeof(data_t));
  for (int ii = 0; ii < size; ii++)
  {
    svBitVecVal temp;
    svGetBitArrElem1VecVal(&temp,src1, ii);
    src1_copy_dynamic[ii] = temp;
    svGetBitArrElem1VecVal(&temp,src2, ii);
    src2_copy_dynamic[ii] = temp;
  }
}

void step_dynamic()
{
  result_dynamic[ind] = src1_copy_dynamic[ind] + src2_copy_dynamic[ind];
  last_commit = result_dynamic[ind];
  ind++;
}


void test_scanf_printf(){
  
  int a;
  printf("cpp tarafindan sayi girilmesi bekleniyor: ");
  fflush(stdout);
  int b = scanf("%d", &a);
  printf("b: %d; a: %d\n", b, a);
  fflush(stdout);
  fflush(stdin);
}
