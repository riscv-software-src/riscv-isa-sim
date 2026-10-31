// See LICENSE for license details.

// Semihosting test program for spike (riscv/semihosting.cc).
// Every check compares a return value or output data with the expected one
// and exits with a non-zero status on mismatch.

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/times.h>

#define FNAME "semihosting_test.tmp"

// libsemihost has no time hooks, so provide the newlib syscall stubs that
// time() and clock() need on top of SYS_TIME and SYS_CLOCK.
static long semihost_call(long op, long arg)
{
  register long a0 asm("a0") = op;
  register long a1 asm("a1") = arg;
  // spike loads the words around ebreak. To test whether it works
  // even when it is not word-aligned, we force it to be 16-bit aligned.
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

#define SYS_ISERROR 0x08
#define SYS_TMPNAM 0x0d
#define SYS_REMOVE 0x0e
#define SYS_RENAME 0x0f
#define SYS_CLOCK 0x10
#define SYS_TIME 0x11
#define SYS_SYSTEM 0x12
#define SYS_ERRNO 0x13
#define SYS_GET_CMDLINE 0x15
#define SYS_HEAPINFO 0x16
#define SYS_EXIT_EXTENDED 0x20
#define SYS_ELAPSED 0x30
#define SYS_TICKFREQ 0x31
#define ADP_Stopped_ApplicationExit 0x20026

// newlib's _exit() always reports success, so use SYS_EXIT_EXTENDED to
// propagate a failure status to spike's exit code.
static void fail_exit(int status)
{
  long args[2] = {ADP_Stopped_ApplicationExit, status};
  semihost_call(SYS_EXIT_EXTENDED, (long)args);
  for (;;)
    ;
}

int _gettimeofday(struct timeval *tv, void *tz)
{
  (void)tz;
  tv->tv_sec = semihost_call(SYS_TIME, 0);
  tv->tv_usec = 0;
  return 0;
}

clock_t _times(struct tms *buf)
{
  clock_t c = semihost_call(SYS_CLOCK, 0); // 1/100 s
  buf->tms_utime = c;
  buf->tms_stime = buf->tms_cutime = buf->tms_cstime = 0;
  return c;
}

static int checks;

// spike traps on misaligned accesses, so avoid word-wise memcmp() on
// arbitrarily aligned buffers.
static int bytes_eq(const void *a, const void *b, size_t n)
{
  const volatile unsigned char *p = a, *q = b;
  while (n--)
    if (*p++ != *q++)
      return 0;
  return 1;
}

#define CHECK(cond)                                                          \
  do {                                                                       \
    checks++;                                                                \
    if (!(cond)) {                                                           \
      fprintf(stderr, "FAIL %s:%d: %s (errno=%d)\n", __FILE__, __LINE__,     \
              #cond, errno);                                                 \
      fail_exit(1);                                                          \
    }                                                                        \
  } while (0)

// SYS_WRITE on the console (":tt")
static void test_console(void)
{
  const char msg[] = "console write\n";
  CHECK(write(1, msg, sizeof(msg) - 1) == (ssize_t)(sizeof(msg) - 1));
  CHECK(printf("printf %d %s\n", 42, "ok") == 13);
  CHECK(fflush(stdout) == 0);
}

// SYS_OPEN (w) / SYS_WRITE / SYS_CLOSE / SYS_FLEN
static void test_write_file(const char *data, size_t len)
{
  int fd = open(FNAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  CHECK(fd >= 3);
  CHECK(write(fd, data, len) == (ssize_t)len);
  struct stat st;
  CHECK(fstat(fd, &st) == 0);
  CHECK((size_t)st.st_size == len);
  CHECK(close(fd) == 0);
}

// SYS_OPEN (r) / SYS_READ / SYS_SEEK / SYS_FLEN
static void test_read_seek(const char *data, size_t len)
{
  char buf[64];
  int fd = open(FNAME, O_RDONLY);
  CHECK(fd >= 3);

  struct stat st;
  CHECK(fstat(fd, &st) == 0);
  CHECK((size_t)st.st_size == len);

  memset(buf, 0, sizeof(buf));
  CHECK(read(fd, buf, len) == (ssize_t)len);
  CHECK(bytes_eq(buf, data, len));

  // EOF: nothing is read
  CHECK(read(fd, buf, sizeof(buf)) == 0);

  CHECK(lseek(fd, 6, SEEK_SET) == 6);
  memset(buf, 0, sizeof(buf));
  CHECK(read(fd, buf, 5) == 5);
  CHECK(bytes_eq(buf, data + 6, 5));

  CHECK(lseek(fd, 0, SEEK_SET) == 0);
  CHECK(read(fd, buf, 5) == 5);
  CHECK(bytes_eq(buf, data, 5));

  CHECK(close(fd) == 0);
}

// SYS_OPEN (a) / SYS_WRITE: append
static void test_append(const char *data, size_t len, const char *extra)
{
  size_t elen = strlen(extra);
  char buf[64];
  int fd = open(FNAME, O_WRONLY | O_CREAT | O_APPEND, 0666);
  CHECK(fd >= 3);
  CHECK(write(fd, extra, elen) == (ssize_t)elen);
  CHECK(close(fd) == 0);

  fd = open(FNAME, O_RDONLY);
  CHECK(fd >= 3);
  memset(buf, 0, sizeof(buf));
  CHECK(read(fd, buf, sizeof(buf)) == (ssize_t)(len + elen));
  CHECK(bytes_eq(buf, data, len));
  CHECK(bytes_eq(buf + len, extra, elen));
  CHECK(close(fd) == 0);
}

// SYS_OPEN (r+) / SYS_SEEK / SYS_WRITE / SYS_READ
static void test_read_write(void)
{
  char buf[16];
  int fd = open(FNAME, O_RDWR);
  CHECK(fd >= 3);
  CHECK(lseek(fd, 0, SEEK_SET) == 0);
  CHECK(write(fd, "HELLO", 5) == 5);
  CHECK(lseek(fd, 0, SEEK_SET) == 0);
  memset(buf, 0, sizeof(buf));
  CHECK(read(fd, buf, 5) == 5);
  CHECK(bytes_eq(buf, "HELLO", 5));
  CHECK(close(fd) == 0);
}

// stdio layer on top of the same APIs
static void test_stdio(void)
{
  char buf[32];
  FILE *fp = fopen(FNAME, "w+");
  CHECK(fp != NULL);
  CHECK(fputs("line1\nline2\n", fp) >= 0);
  CHECK(fprintf(fp, "%d-%s\n", 123, "abc") == 8);
  CHECK(fflush(fp) == 0);
  CHECK(fseek(fp, 0, SEEK_SET) == 0);
  CHECK(fgets(buf, sizeof(buf), fp) != NULL);
  CHECK(strcmp(buf, "line1\n") == 0);
  CHECK(fgets(buf, sizeof(buf), fp) != NULL);
  CHECK(strcmp(buf, "line2\n") == 0);
  int n = 0;
  CHECK(fscanf(fp, "%d-%31s", &n, buf) == 2);
  CHECK(n == 123);
  CHECK(strcmp(buf, "abc") == 0);
  CHECK(fseek(fp, 0, SEEK_END) == 0);
  CHECK(ftell(fp) == 20);
  CHECK(fclose(fp) == 0);
}

// SYS_OPEN returns -1 for a missing file
static void test_errors(void)
{
  CHECK(open("no_such_dir/no_such_file", O_RDONLY) == -1);
  CHECK(fopen("no_such_dir/no_such_file", "r") == NULL);
  CHECK(close(-1) == -1);
}

// SYS_ISERROR reports whether a signed status value indicates an error.
static void test_iserror(void)
{
  long args[] = {0};
  CHECK(semihost_call(SYS_ISERROR, (long)args) == 0);
  args[0] = 1;
  CHECK(semihost_call(SYS_ISERROR, (long)args) == 0);
  args[0] = -1;
  CHECK(semihost_call(SYS_ISERROR, (long)args) == 1);
}

// SYS_GET_CMDLINE copies target arguments as a space-separated string.
static void test_get_cmdline(void)
{
  static const char suffix[] = " cmdline-one cmdline-two";
  char buf[128];
  long args[2] = {(long)buf, sizeof(buf)};

  memset(buf, 0xa5, sizeof(buf));
  CHECK(semihost_call(SYS_GET_CMDLINE, (long)args) == 0);
  CHECK(args[1] > sizeof(suffix));
  CHECK(args[1] <= sizeof(buf));
  CHECK(buf[args[1] - 1] == '\0');
  CHECK(bytes_eq(buf + args[1] - sizeof(suffix), suffix, sizeof(suffix)));

  args[1] = sizeof(suffix) - 1;
  CHECK(semihost_call(SYS_GET_CMDLINE, (long)args) == -1);
  CHECK(semihost_call(SYS_ERRNO, 0) == EINVAL);
}

// SYS_ISTTY: a regular file is not a tty
static void test_isatty(void)
{
  int fd = open(FNAME, O_RDONLY);
  CHECK(fd >= 3);
  CHECK(isatty(fd) == 0);
  CHECK(close(fd) == 0);
}

// SYS_REMOVE / SYS_RENAME via unlink() and raw semihosting calls
static void test_remove_rename(void)
{
  const char *a = "semihosting_a.tmp", *b = "semihosting_b.tmp";
  char buf[8];
  int fd = open(a, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  CHECK(fd >= 3);
  CHECK(write(fd, "abc", 3) == 3);
  CHECK(close(fd) == 0);

  long ren[4] = {(long)a, (long)strlen(a), (long)b, (long)strlen(b)};
  CHECK(semihost_call(SYS_RENAME, (long)ren) == 0);
  CHECK(open(a, O_RDONLY) == -1);
  fd = open(b, O_RDONLY);
  CHECK(fd >= 3);
  CHECK(read(fd, buf, sizeof(buf)) == 3);
  CHECK(bytes_eq(buf, "abc", 3));
  CHECK(close(fd) == 0);

  // renaming a missing file fails and errno is reported by SYS_ERRNO
  CHECK(semihost_call(SYS_RENAME, (long)ren) == -1);
  CHECK(semihost_call(SYS_ERRNO, 0) == ENOENT);

  CHECK(unlink(b) == 0);
  CHECK(open(b, O_RDONLY) == -1);

  long rm[2] = {(long)b, (long)strlen(b)};
  CHECK(semihost_call(SYS_REMOVE, (long)rm) == -1);
  CHECK(semihost_call(SYS_ERRNO, 0) == ENOENT);
}

// SYS_SYSTEM: the host shell runs in spike's working directory
static void test_system(void)
{
  const char *ok = "true", *ng = "exit 3";
  const char *wr = "echo semi > semihosting_sys.tmp";
  char buf[8] = {0};
  long a[2] = {(long)ok, (long)strlen(ok)};
  CHECK(semihost_call(SYS_SYSTEM, (long)a) == 0);

  a[0] = (long)ng;
  a[1] = (long)strlen(ng);
  CHECK(semihost_call(SYS_SYSTEM, (long)a) == (3 << 8)); // raw system() status

  a[0] = (long)wr;
  a[1] = (long)strlen(wr);
  CHECK(semihost_call(SYS_SYSTEM, (long)a) == 0);
  int fd = open("semihosting_sys.tmp", O_RDONLY);
  CHECK(fd >= 3);
  CHECK(read(fd, buf, sizeof(buf)) == 5);
  CHECK(bytes_eq(buf, "semi\n", 5));
  CHECK(close(fd) == 0);
  CHECK(unlink("semihosting_sys.tmp") == 0);
}

// SYS_TMPNAM returns a name without creating a file.
static void test_tmpnam(void)
{
  static char name[4096], other[4096];
  long args[3] = {(long)name, 0, sizeof(name)};

  memset(name, 0x5a, sizeof(name));
  CHECK(semihost_call(SYS_TMPNAM, (long)args) == 0);
  CHECK(name[0] != '\0');
  CHECK(memchr(name, '\0', sizeof(name)) != NULL);
  CHECK(name[sizeof(name) - 1] == 0x5a);
  CHECK(open(name, O_RDONLY) == -1);
  CHECK(errno == ENOENT);

  args[0] = (long)other;
  args[1] = 0;
  CHECK(semihost_call(SYS_TMPNAM, (long)args) == 0);
  CHECK(strcmp(name, other) == 0);

  args[1] = 255;
  CHECK(semihost_call(SYS_TMPNAM, (long)args) == 0);
  CHECK(memchr(other, '\0', sizeof(other)) != NULL);
  CHECK(strcmp(name, other) != 0);

  memset(other, 0x5a, sizeof(other));
  args[1] = 256;
  CHECK(semihost_call(SYS_TMPNAM, (long)args) == -1);
  CHECK(semihost_call(SYS_ERRNO, 0) == EINVAL);
  CHECK(other[0] == 0x5a);

  args[1] = -1;
  CHECK(semihost_call(SYS_TMPNAM, (long)args) == -1);
  CHECK(semihost_call(SYS_ERRNO, 0) == EINVAL);
  CHECK(other[0] == 0x5a);

  args[1] = 0;
  args[2] = 0;
  CHECK(semihost_call(SYS_TMPNAM, (long)args) == -1);
  CHECK(semihost_call(SYS_ERRNO, 0) == EINVAL);
  CHECK(other[0] == 0x5a);
}

// Unimplemented operations return -1 with errno == ENOSYS
static void test_unimplemented(void)
{
  static const long ops[] = {SYS_HEAPINFO, 0x7f /*unknown*/};
  long arg[4] = {0, 0, 0, 0};
  for (unsigned i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
    CHECK(semihost_call(ops[i], (long)arg) == -1);
    CHECK(semihost_call(SYS_ERRNO, 0) == ENOSYS);
  }
}

static void busy_loop(unsigned count)
{
  volatile unsigned x = 0;
  for (unsigned i = 0; i < count; i++)
    x += i;
}

// SYS_TICKFREQ / SYS_ELAPSED
static void test_ticks(void)
{
  long freq = semihost_call(SYS_TICKFREQ, 0);
  CHECK(freq > 0);

  unsigned long long e0 = 0, e1 = 0;
#if __riscv_xlen == 32
  unsigned long w0[2] = {0, 0}, w1[2] = {0, 0};
  CHECK(semihost_call(SYS_ELAPSED, (long)w0) == 0);
  busy_loop(1000);
  CHECK(semihost_call(SYS_ELAPSED, (long)w1) == 0);
  e0 = ((unsigned long long)w0[1] << 32) | w0[0];
  e1 = ((unsigned long long)w1[1] << 32) | w1[0];
#else
  CHECK(semihost_call(SYS_ELAPSED, (long)&e0) == 0);
  busy_loop(1000);
  CHECK(semihost_call(SYS_ELAPSED, (long)&e1) == 0);
#endif
  CHECK(e1 > e0);
}

// SYS_TIME / SYS_CLOCK
static void test_time(void)
{
  time_t t = time(NULL);
  CHECK(t > 1600000000); // later than 2020-09-13

  clock_t c0 = clock();
  CHECK(c0 != (clock_t)-1);
  busy_loop(100000);
  clock_t c1 = clock();
  CHECK(c1 >= c0);
}

int main(void)
{
  static const char data[] = "hello world semihosting";
  const size_t len = sizeof(data) - 1;

  test_console();
  test_write_file(data, len);
  test_read_seek(data, len);
  test_append(data, len, " appended");
  test_read_write();
  test_stdio();
  test_isatty();
  test_errors();
  test_iserror();
  test_get_cmdline();
  test_remove_rename();
  test_system();
  test_tmpnam();
  test_unimplemented();
  test_ticks();
  test_time();

  CHECK(unlink(FNAME) == 0);

  printf("PASS (%d checks)\n", checks);
  fflush(stdout);
  return 0; // SYS_EXIT with status 0
}
