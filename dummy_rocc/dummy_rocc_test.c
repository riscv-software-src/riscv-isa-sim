// The following is a RISC-V program to test the functionality of the
// dummy RoCC accelerator.
// Compile with riscv64-unknown-elf-gcc dummy_rocc_test.c
// Run with spike --extension=dummy_rocc pk a.out

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

int main() {
  uint64_t x = 123, y = 456, z = 0;
  // load x into accumulator 2 (funct=0)
  asm volatile ("custom0 x0, %0, 2, 0" : : "r"(x));
  // read it back into z (funct=1) to verify it
  asm volatile ("custom0 %0, x0, 2, 1" : "=r"(z));
  assert(z == x);
  // accumulate 456 into it (funct=3)
  asm volatile ("custom0 x0, %0, 2, 3" : : "r"(y));
  // verify it
  asm volatile ("custom0 %0, x0, 2, 1" : "=r"(z));
  assert(z == x+y);
  // do it all again, but initialize acc2 via memory this time (funct=2)
  asm volatile ("custom0 x0, %0, 2, 2" : : "r"(&x));
  asm volatile ("custom0 x0, %0, 2, 3" : : "r"(y));
  asm volatile ("custom0 %0, x0, 2, 1" : "=r"(z));
  assert(z == x+y);

  printf("success!\n");
}
