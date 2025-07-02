#include <stdio.h>
#include <stdatomic.h>

atomic_int acnt = 0;
atomic_int bcnt = 0;

int foo() {
  for(int n = 0; n < 1000; ++n) {
    ++acnt;
    if(acnt % 10 == 0)
      ++bcnt;
  }
  return acnt;
}

int main(void) {
  int acnt = foo();
  printf("First atomic counter is %u, second is %u\n", acnt, bcnt);
  return 0;
}
