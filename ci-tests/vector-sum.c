#include <stdio.h>
#include <string.h>
#include <riscv_vector.h>

int main()
{
  const size_t N = 1001;
  volatile int a[N];
  int* b = (int*)a;

  for (size_t i = 0; i < N; i++)
    a[i] = i;

  vint32m8_t vsum = __riscv_vmv_v_x_i32m8(0, -1);
  for (size_t i = 0; i < N; ) {
    size_t vl = __riscv_vsetvl_e32m8(N - i);
    vint32m8_t v = __riscv_vle32_v_i32m8(b + i, vl);
    vsum = __riscv_vadd_vv_i32m8(vsum, v, vl);
    i += vl;
  }
  vint32m1_t vzero = __riscv_vmv_v_x_i32m1(0, 1);
  int sum = __riscv_vmv_x_s_i32m1_i32(__riscv_vredsum_vs_i32m8_i32m1(vsum, vzero, -1));

  printf("The sum of the first %zu positive integers is %d.\n", N - 1, sum);

  return 0;
}
