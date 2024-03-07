#include <stdio.h>

int main() {
  // as if slli.uw zero, t1, 3
  asm(".4byte 0x0833101b");
  printf("Executed successfully\n");
  return 0;
}
