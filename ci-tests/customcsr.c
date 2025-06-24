#include <stdio.h>

int main() {
  int x = 1;
  //            dummycsr
  asm("csrr %0, 0xfff" : "=r"(x));
  if (x == 42)
    printf("Executed successfully\n");
  else
    printf("FAIL. Got value: %d instead of 42\n", x);
  return 0;
}
