#include <svdpi.h> // sua n bu header'dan kullandigim tek sey DPI_DLLESPEC
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "../../obj_dir/tb_1__Dpi.h"
#include "../../obj_dir/tb_2__Dpi.h"
// #include "../../obj_dir/Vtb_3__Dpi.h"
// #include "../../obj_dir/Vtb_4__Dpi.h"

typedef int32_t data_t;

int index;
data_t last_commit;

// test 4 c tarafindan verilog tarafindaki array'lere yaz
data_t *src1_glob;
data_t *src2_glob;
data_t *result_glob;
FILE *out_logs, *err_logs;
void init(const svOpenArrayHandle target1, const svOpenArrayHandle target2)
{
  const static data_t edge_cases[][2] = {
      {0, 0},
      {0, -1},
      {-1, -1},
      {-1, 0},
  };

  FILE *temp;
  if ((temp = fopen("C:/Users/123uz/Desktop/tutel/git_repos/system_verilog_dpi/faruk_out.log", "w")) == NULL)
  {
  }
  fclose(temp);
  if ((temp = fopen("C:/Users/123uz/Desktop/tutel/git_repos/system_verilog_dpi/faruk_err.log", "w")) == NULL)
  {
  }
  fclose(temp);
  // files are cleared
  if ((out_logs = fopen("C:/Users/123uz/Desktop/tutel/git_repos/system_verilog_dpi/faruk_out.log", "a")) == NULL)
  {
  }
  if ((err_logs = fopen("C:/Users/123uz/Desktop/tutel/git_repos/system_verilog_dpi/faruk_err.log", "a")) == NULL)
  {
  }
  char time_str[100];
  time_t now = time(NULL);
  strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
  fprintf(out_logs, "---------------------------------\n");
  fprintf(out_logs, "[%s] out_logs ve err_logs olusturuldu\n",time_str);
  fflush(out_logs);
  int size = svSize(target1, 1);

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
    svPutLogicArrElemVecVal (target1,&logic_val,ii);
    logic_val.aval = src2_glob[ii] = edge_cases[ii][1];
    svPutLogicArrElemVecVal (target2,&logic_val,ii);
  }
  srand(0u);
  for (; ii < edge_case_size + num_remaining_slots; ii++)
  {
    logic_val.aval = src1_glob[ii] = rand() % 10;
    svPutLogicArrElemVecVal (target1,&logic_val,ii);
    logic_val.aval = src2_glob[ii] = rand() % 10;
    svPutLogicArrElemVecVal (target2,&logic_val,ii);
  }
}

void step_glob(){
  result_glob[index] = src1_glob[index] + src2_glob[index];
  last_commit = result_glob[index];
  index++;
}
// test 3 c tarafinda array'leri malloc'la olustur
data_t *src1_copy_dynamic;
data_t *src2_copy_dynamic;
data_t *result_dynamic;
void c_init_dynamic(const svOpenArrayHandle src1, const svOpenArrayHandle src2)
{
  int size = svSize(src1, 1);
  src1_copy_dynamic = (data_t *)malloc(size * sizeof(data_t));
  src2_copy_dynamic = (data_t *)malloc(size * sizeof(data_t));
  result_dynamic = (data_t *)malloc(size * sizeof(data_t));

  for (int ii = 0; ii < size; ii++)
  {
    src1_copy_dynamic[ii] = *((int *)svGetArrElemPtr1(src1, ii));
    src2_copy_dynamic[ii] = *((int *)svGetArrElemPtr1(src2, ii));
  }
}

void step_dynamic()
{
  result_dynamic[index] = src1_copy_dynamic[index] + src2_copy_dynamic[index];
  last_commit = result_dynamic[index];
  index++;
}

// test 2 array'leri kulaln
#define SIZE 8
data_t src1_copy[SIZE];
data_t src2_copy[SIZE];
data_t result[SIZE];

void c_init(const svOpenArrayHandle src1, const svOpenArrayHandle src2)
{
  for (int ii = 0; ii < SIZE; ii++)
  {
    src1_copy[ii] = *((int *)svGetArrElemPtr1(src1, ii));
    src2_copy[ii] = *((int *)svGetArrElemPtr1(src2, ii));
  }
}

void step()
{
  last_commit = result[index] = src1_copy[index] + src2_copy[index];
  index++;
}

int get_last_commit()
{
  return last_commit;
}

// test 1 tekli deneme
int test_c(int operand1, int operand2)
{
  return operand1 + operand2;
}
