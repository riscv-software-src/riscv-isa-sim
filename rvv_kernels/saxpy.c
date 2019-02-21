#include <stdio.h>
#include <stdlib.h>

//#include "util.h"
#include "dataset1.h"

void vec_saxpy_asm(size_t n, const float a, const float* x, float* y);

void saxpy_c(size_t n, const float a, const float* x, float* y)
{
  for (size_t i=0; i<n; i++) {
    y[i] = a * x[i] + y[i];
  }
}

int verifyFloat(int sz, float *x, float *y){
    int res = 0;
    for (size_t i=0; i<sz; ++i){
        float fx = *x, fy = *y;
        if (fx != fy){
            //printf("Fail at %ld\n", i);
            res++;
        }
    }
    return res;
}

int main(int argc, char **argv)
{
  int loops=1;
  if (argc == 2) loops = atoi(argv[1]);
  for (int i=0; i<loops; ++i){
    vec_saxpy_asm(DATA_SIZE, input_data_a, input_data_X, input_data_Y);
  }
  int failures = verifyFloat(DATA_SIZE, input_data_Y, verify_data);
  if (failures == 0) {
	  printf("PASSED!\n");
  } else {
	  printf("FAILED (%d)!\n", failures);
  }
  return 0;
}
