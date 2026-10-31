// See LICENSE for license details.

// Test the semihosting debug-console APIs.

#include <stdio.h>

#define SYS_WRITEC 0x03
#define SYS_WRITE0 0x04
#define SYS_READC 0x07
#define SYS_EXIT_EXTENDED 0x20
#define ADP_Stopped_ApplicationExit 0x20026

static long semihost_call(long op, long arg)
{
  register long a0 asm("a0") = op;
  register long a1 asm("a1") = arg;
  asm volatile(".balign 16\n"
               "c.nop\n"
               ".option push\n.option norvc\n"
               "slli zero, zero, 0x1f\n"
               "ebreak\n"
               "srai zero, zero, 7\n"
               ".option pop"
               : "+r"(a0) : "r"(a1) : "memory");
  return a0;
}

static void fail_exit(void)
{
  long args[2] = {ADP_Stopped_ApplicationExit, 1};
  semihost_call(SYS_EXIT_EXTENDED, (long)args);
  for (;;)
    ;
}

int main(void)
{
  const char writec = 'C';
  const char write0[] = "semihosting WRITE0\n";

  if (semihost_call(SYS_WRITEC, (long)&writec) != 0 ||
      semihost_call(SYS_WRITE0, (long)write0) != 0 ||
      semihost_call(SYS_READC, 0) != 'R') {
    fprintf(stderr, "FAIL debug console API test\n");
    fail_exit();
  }

  return 0;
}
