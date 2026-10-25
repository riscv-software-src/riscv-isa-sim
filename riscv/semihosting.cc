// See LICENSE for license details.

// RISC-V semihosting support
//   https://github.com/riscv-non-isa/riscv-semihosting
//   https://github.com/ARM-software/abi-aa/blob/main/semihosting/semihosting.rst

#include "decode_macros.h"
#include "mmu.h"
#include "processor.h"
#include "sim.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#if 0
#  define D(x) x
#else
#  define D(x) (void) 0
#endif

enum semihosting_op {
  SYS_OPEN          = 0x01,
  SYS_CLOSE         = 0x02,
  SYS_WRITEC        = 0x03,
  SYS_WRITE0        = 0x04,
  SYS_WRITE         = 0x05,
  SYS_READ          = 0x06,
  SYS_READC         = 0x07,
  SYS_ISERROR       = 0x08,
  SYS_ISTTY         = 0x09,
  SYS_SEEK          = 0x0a,
  SYS_FLEN          = 0x0c,
  SYS_TMPNAM        = 0x0d,
  SYS_REMOVE        = 0x0e,
  SYS_RENAME        = 0x0f,
  SYS_CLOCK         = 0x10,
  SYS_TIME          = 0x11,
  SYS_SYSTEM        = 0x12,
  SYS_ERRNO         = 0x13,
  SYS_GET_CMDLINE   = 0x15,
  SYS_HEAPINFO      = 0x16,
  SYS_EXIT          = 0x18,
  SYS_EXIT_EXTENDED = 0x20,
  SYS_ELAPSED       = 0x30,
  SYS_TICKFREQ      = 0x31
};

enum adp_stopped_reason {
  ADP_Stopped_BranchThroughZero     = 0x20000,
  ADP_Stopped_UndefinedInstr        = 0x20001,
  ADP_Stopped_SoftwareInterrupt     = 0x20002,
  ADP_Stopped_PrefetchAbort         = 0x20003,
  ADP_Stopped_DataAbort             = 0x20004,
  ADP_Stopped_AddressException      = 0x20005,
  ADP_Stopped_IRQ                   = 0x20006,
  ADP_Stopped_FIQ                   = 0x20007,
  ADP_Stopped_BreakPoint            = 0x20020,
  ADP_Stopped_WatchPoint            = 0x20021,
  ADP_Stopped_StepComplete          = 0x20022,
  ADP_Stopped_RunTimeErrorUnknown   = 0x20023,
  ADP_Stopped_InternalError         = 0x20024,
  ADP_Stopped_UserInterruption      = 0x20025,
  ADP_Stopped_ApplicationExit       = 0x20026,
  ADP_Stopped_StackOverflow         = 0x20027,
  ADP_Stopped_DivisionByZero        = 0x20028,
  ADP_Stopped_OSSpecific            = 0x20029
};

static FILE *debug_in = stdin;
static FILE *debug_out = stderr;

// Load a value from the target memory
static inline reg_t target_load(processor_t* p, const int xlen, reg_t addr)
{
  if (xlen == 32) {
    return MMU.load<uint32_t>(addr);
  } else {
    return MMU.load<uint64_t>(addr);
  }
}

// Store a value to the target memory
static inline void target_store(processor_t* p, const int xlen, reg_t addr, reg_t data) {
  if (xlen == 32) {
    MMU.store<uint32_t>(addr, data);
  } else {
    MMU.store<uint64_t>(addr, data);
  }
}

#define MMU_load(addr) target_load(p, xlen, addr)
#define MMU_store(addr, data) target_store(p, xlen, addr, data)

// memcpy from target (spike) to host
static void *memcpy_t2h(processor_t* p, void *dst, const reg_t src, size_t n)
{
  for (size_t i = 0; i < n; ++i) {
    auto c = MMU.load<uint8_t>(src + i);
    ((uint8_t *)dst)[i] = c;
  }
  return dst;
}

// memcpy from host to target (spike)
static reg_t memcpy_h2t(processor_t* p, reg_t dst, const void *src, size_t n)
{
  for (size_t i = 0; i < n; ++i) {
    MMU.store<uint8_t>(dst + i, ((const uint8_t *)src)[i]);
  }
  return dst;
}

// convert semihosting flags to host open mode
static int open_oflag(uint32_t flags)
{
  int m = 0;
  switch (flags >> 1) { // ignore "b" in mode string
    case 0: m = O_RDONLY; break;  // r
    case 1: m = O_RDWR;   break;  // r+
    case 2: m = O_WRONLY | O_CREAT | O_TRUNC;  break;   // w
    case 3: m = O_RDWR   | O_CREAT | O_TRUNC;  break;   // w+
    case 4: m = O_WRONLY | O_CREAT | O_APPEND; break;   // a
    case 5: m = O_RDWR   | O_CREAT | O_APPEND; break;   // a+
    default: m = -1; break; // unknown mode
  }
  return m;
}

// Return value: a file descriptor if successful, otherwise -1
static int sys_open(processor_t* p, const int xlen, reg_t a1)
{
  int fd;

  reg_t fp = MMU_load(a1);
  int flags = MMU_load(a1 + xlen/8);
  reg_t length = MMU_load(a1 + 2 * xlen/8);

  if (length >= PATH_MAX) {
    errno = EINVAL;
    return -1;
  }

  std::vector<char> fname(length + 1);
  memcpy_t2h(p, fname.data(), fp, length);
  fname[length] = 0;

  if (strcmp(fname.data(), ":tt") == 0) {
    // Handle TTY case
    fd = flags >> 2;   // 0: stdin, 1: stdout, 2: stderr
    if (fd < 0 || fd > 2) {
      errno = EBADF;
      return -1;
    }
  } else {
    int oflag = open_oflag(flags);
    if (oflag == -1) {
      errno = EINVAL;
      return -1;
    }
    fd = open(fname.data(), oflag, 0666);
  }
  D(fprintf(stderr, "sys_open:fname: %s, fd: %d\n", fname.data(), fd));
  return fd;
}

// Return value: 0 if successful, -1 on error
static int sys_close(processor_t* p, const int xlen, reg_t a1)
{
  int fd = MMU_load(a1);

  if (fd == 0 || fd == 1 || fd == 2) {
    return 0; // Do not close standard input/output/error
  } else {
    return close(fd);
  }
}

// Return value: None
static int sys_writec(processor_t* p, reg_t a1)
{
  unsigned char c = MMU.load<unsigned char>(a1);
  putc(c, debug_out);
  return 0;
}

// Return value: None
static int sys_write0(processor_t* p, reg_t a1)
{
  unsigned char c;
  for (reg_t sp = a1; (c = MMU.load<unsigned char>(sp)); sp++) {
    putc(c, debug_out);
  }
  return 0;
}

// Return value: 0 if successful. The number of bytes that are not written, -1 on error.
static int sys_write(processor_t* p, const int xlen, reg_t a1)
{
  int fd = MMU_load(a1);
  reg_t bp = MMU_load(a1 + xlen/8);
  size_t length = MMU_load(a1 + 2 * xlen/8);

  std::vector<char> buf(length);
  memcpy_t2h(p, buf.data(), bp, length);
  ssize_t ret = write(fd, buf.data(), length);
  return ret < 0 ? -1 : (int)(length - (size_t)ret);
}

// Return value: the byte read from the console, or EOF on error
static int sys_readc()
{
  int c = getc(debug_in);
  return c;
}

// Return value: the number of bytes not filled in the buffer
static int sys_read(processor_t* p, const int xlen, reg_t a1)
{
  int fd = MMU_load(a1);
  reg_t bp = MMU_load(a1 + xlen/8);
  size_t length = MMU_load(a1 + 2 * xlen/8);

  std::vector<char> buf(length);
  ssize_t ret = read(fd, buf.data(), length);
  if (ret < 0)
    return -1;
  memcpy_h2t(p, bp, buf.data(), ret);
  return length - (size_t)ret;
}

// Return value: 0 if the status field is not an error indication
// 1 if the status field is an error indication.
static int sys_iserror(processor_t* p, const int xlen, reg_t a1)
{
  int status = MMU_load(a1);
  return status < 0;
}

// Return value: 1 if the file descriptor is associated with a terminal, otherwise 0, -1 on error
static int sys_isatty(processor_t* p, const int xlen, reg_t a1)
{
  int fd = MMU_load(a1);
  return isatty(fd) ? 1 : (errno == ENOTTY ? 0 : -1);
}

// Return value: 0 if successful. -1 on error.
static off_t sys_seek(processor_t* p, const int xlen, reg_t a1)
{
  int fd = MMU_load(a1);
  off_t offset = MMU_load(a1 + xlen/8);

  off_t ret = lseek(fd, offset, SEEK_SET);
  return ret < 0 ? -1 : 0;
}

// Return value: length of the file in bytes, -1 if an error occurs
static off_t sys_flen(processor_t* p, const int xlen, reg_t a1)
{
  struct stat st;
  int fd = MMU_load(a1);

  if (fstat(fd, &st) == 0) {
    return st.st_size;
  } else {
    return -1;
  }
}

// Return value: 0 if successful. -1 on error.
static int sys_tmpnam(processor_t* p, const int xlen, reg_t a1)
{
  const int MAX_TARGETS = 255;

  reg_t buf = MMU_load(a1);
  reg_t target_id = MMU_load(a1 + xlen/8);
  int length = MMU_load(a1 + 2 * xlen/8);

  if (target_id > MAX_TARGETS || length < L_tmpnam) {
    errno = EINVAL;
    return -1;
  }

  std::vector<char> path_name(length);
  int len = snprintf(path_name.data(), length, "%s/spike%x_%02x",
                     std::filesystem::temp_directory_path().c_str(),
                     (unsigned int)getpid(), (unsigned int)target_id);
  if (len < 0 || len + 1 > length) {
    errno = EINVAL;
    return -1;
  }
  memcpy_h2t(p, buf, path_name.data(), len + 1);
  return 0;
}

// Return value: 0 if successful. -1 on error.
static int sys_remove(processor_t* p, const int xlen, reg_t a1)
{
  reg_t pname = MMU_load(a1);
  reg_t length = MMU_load(a1 + xlen/8);

  std::vector<char> path_name(length + 1);
  memcpy_t2h(p, path_name.data(), pname, length);
  path_name[length] = 0;

  return unlink(path_name.data());
}

// Return value: 0 if successful. -1 on error.
static int sys_rename(processor_t* p, const int xlen, reg_t a1)
{
  reg_t oldname = MMU_load(a1);
  reg_t length_old = MMU_load(a1 + xlen/8);
  reg_t newname = MMU_load(a1 + 2 * xlen/8);
  reg_t length_new = MMU_load(a1 + 3 * xlen/8);

  std::vector<char> old_path(length_old + 1);
  std::vector<char> new_path(length_new + 1);
  memcpy_t2h(p, old_path.data(), oldname, length_old);
  memcpy_t2h(p, new_path.data(), newname, length_new);
  old_path[length_old] = 0;
  new_path[length_new] = 0;

  return rename(old_path.data(), new_path.data());
}

// no arguments, Returns the number of seconds since 00:00 January 1, 1970.
static time_t sys_time()
{
  return time(nullptr);
}

// Return value: the return status.
static int sys_system(processor_t* p, const int xlen, reg_t a1)
{
  reg_t cmd = MMU_load(a1);
  reg_t length = MMU_load(a1 + xlen/8);

  std::vector<char> command(length + 1);
  memcpy_t2h(p, command.data(), cmd, length);
  command[length] = 0;

  return system(command.data());
}

// Convert a host errno value to the target (newlib/picolibc) numbering
//   tested only on Linux and macOS
static int errno_h2t(int e)
{
  switch (e) {
    case 0:               return 0;
    case EPERM:           return 1;
    case ENOENT:          return 2;
    case ESRCH:           return 3;
    case EINTR:           return 4;
    case EIO:             return 5;
    case ENXIO:           return 6;
    case E2BIG:           return 7;
    case ENOEXEC:         return 8;
    case EBADF:           return 9;
    case ECHILD:          return 10;
    case EAGAIN:          return 11;
    case ENOMEM:          return 12;
    case EACCES:          return 13;
    case EFAULT:          return 14;
    case ENOTBLK:         return 15;
    case EBUSY:           return 16;
    case EEXIST:          return 17;
    case EXDEV:           return 18;
    case ENODEV:          return 19;
    case ENOTDIR:         return 20;
    case EISDIR:          return 21;
    case EINVAL:          return 22;
    case ENFILE:          return 23;
    case EMFILE:          return 24;
    case ENOTTY:          return 25;
    case ETXTBSY:         return 26;
    case EFBIG:           return 27;
    case ENOSPC:          return 28;
    case ESPIPE:          return 29;
    case EROFS:           return 30;
    case EMLINK:          return 31;
    case EPIPE:           return 32;
    case EDOM:            return 33;
    case ERANGE:          return 34;
    case ENOMSG:          return 35;
    case EIDRM:           return 36;
#if __linux__
    case ECHRNG:          return 37;
    case EL2NSYNC:        return 38;
    case EL3HLT:          return 39;
    case EL3RST:          return 40;
    case ELNRNG:          return 41;
    case EUNATCH:         return 42;
    case ENOCSI:          return 43;
    case EL2HLT:          return 44;
#endif
    case EDEADLK:         return 45;
    case ENOLCK:          return 46;
#if __linux__
    case EBADE:           return 50;
    case EBADR:           return 51;
    case EXFULL:          return 52;
    case ENOANO:          return 53;
    case EBADRQC:         return 54;
    case EBADSLT:         return 55;
    // #define EDEADLOCK EDEADLK
    //case EDEADLOCK:       return 56;
    case EBFONT:          return 57;
#endif
    case ENOSTR:          return 60;
    case ENODATA:         return 61;
    case ETIME:           return 62;
    case ENOSR:           return 63;
#if __linux__
    case ENONET:          return 64;
    case ENOPKG:          return 65;
#endif
    case EREMOTE:         return 66;
    case ENOLINK:         return 67;
#if __linux__
    case EADV:            return 68;
    case ESRMNT:          return 69;
    case ECOMM:           return 70;
#endif
    case EPROTO:          return 71;
    case EMULTIHOP:       return 74;
#if !__linux__ && !__MACH__
    case ELBIN:           return 75;
#endif
#if __linux__
    case EDOTDOT:         return 76;
#endif
    case EBADMSG:         return 77;
#if !__linux__
    case EFTYPE:          return 79;
#endif
#if __linux__
    case ENOTUNIQ:        return 80;
    case EBADFD:          return 81;
    case EREMCHG:         return 82;
    case ELIBACC:         return 83;
    case ELIBBAD:         return 84;
    case ELIBSCN:         return 85;
    case ELIBMAX:         return 86;
    case ELIBEXEC:        return 87;
#endif
    case ENOSYS:          return 88;
    case ENOTEMPTY:       return 90;
    case ENAMETOOLONG:    return 91;
    case ELOOP:           return 92;
    case EOPNOTSUPP:      return 95;
    case EPFNOSUPPORT:    return 96;
    case ECONNRESET:      return 104;
    case ENOBUFS:         return 105;
    case EAFNOSUPPORT:    return 106;
    case EPROTOTYPE:      return 107;
    case ENOTSOCK:        return 108;
    case ENOPROTOOPT:     return 109;
    case ESHUTDOWN:       return 110;
    case ECONNREFUSED:    return 111;
    case EADDRINUSE:      return 112;
    case ECONNABORTED:    return 113;
    case ENETUNREACH:     return 114;
    case ENETDOWN:        return 115;
    case ETIMEDOUT:       return 116;
    case EHOSTDOWN:       return 117;
    case EHOSTUNREACH:    return 118;
    case EINPROGRESS:     return 119;
    case EALREADY:        return 120;
    case EDESTADDRREQ:    return 121;
    case EMSGSIZE:        return 122;
    case EPROTONOSUPPORT: return 123;
    case ESOCKTNOSUPPORT: return 124;
    case EADDRNOTAVAIL:   return 125;
    case ENETRESET:       return 126;
    case EISCONN:         return 127;
    case ENOTCONN:        return 128;
    case ETOOMANYREFS:    return 129;
#if !__linux__ && !__MACH__
    case EPROCLIM:        return 130;
#endif
    case EUSERS:          return 131;
    case EDQUOT:          return 132;
    case ESTALE:          return 133;
#if !__linux__
    // #define ENOTSUP EOPNOTSUPP
    case ENOTSUP:         return 134;
#endif
#if __linux__
    case ENOMEDIUM:       return 135;
#endif
    case EILSEQ:          return 138;
    case EOVERFLOW:       return 139;
    case ECANCELED:       return 140;
    case ENOTRECOVERABLE: return 141;
    case EOWNERDEAD:      return 142;
#if __linux__
    case ESTRPIPE:        return 143;
    case EHWPOISON:       return 144;
    case EISNAM:          return 145;
    case EKEYEXPIRED:     return 146;
    case EKEYREJECTED:    return 147;
    case EKEYREVOKED:     return 148;
#endif
    // #define EWOULDBLOCK EAGAIN
    // case EWOULDBLOCK:     return EAGAIN;
    default:              return e;
  }
}

// Return value: the current errno value (target numbering)
static int sys_errno()
{
  return errno_h2t(errno);
}

// Return value: 0 if successful, -1 on error
static int sys_get_cmdline(processor_t* p, const int xlen, reg_t a1,
    const std::vector<std::string>& target_args)
{
  reg_t bp = MMU_load(a1);
  size_t length = MMU_load(a1 + xlen/8);

  std::vector<char> buf(length);
  size_t len = 0;
  for (const auto& arg : target_args) {
    size_t arg_len = arg.size() + 1;
    if (len + arg_len > length) {
        errno = EINVAL;
        return -1;
    }
    memcpy(buf.data() + len, arg.c_str(), arg_len);
    len += arg_len;
    buf[len - 1] = ' ';
  }
  buf[len - 1] = '\0';
  memcpy_h2t(p, bp, buf.data(), len);
  MMU_store(a1 + xlen/8, len);
  return 0;
}

static int sys_exit_extended(processor_t* p, const int xlen, reg_t a1)
{
  int exception_type = MMU_load(a1);
  int subcode = MMU_load(a1 + xlen/8);

  return exception_type == ADP_Stopped_ApplicationExit ? subcode : exception_type;
}

static int sys_exit(processor_t* p, const int xlen, reg_t a1)
{
  if (xlen == 64) {
    return sys_exit_extended(p, xlen, a1);
  }

  int exception_type = a1;
  return exception_type == ADP_Stopped_ApplicationExit ? 0 : exception_type;
}

static uint64_t elapsed(processor_t* p)
{
  return p->get_state()->mcycle->read();
}

// Returns the number of elapsed target ticks since execution started.
// Return value: always 0 (success)
static int sys_elapsed(processor_t* p, const int xlen, reg_t a1)
{
  uint64_t u = elapsed(p);
  if (xlen == 32) {
    MMU_store(a1, u);
    MMU_store(a1 + xlen/8, u >> 32);
  } else {
    MMU_store(a1, u);
  }
  return 0;
}

// Return value: the frequency of the system tick in Hz
static int sys_tickfreq()
{
  return sim_t::CPU_HZ;
}

// Return value: clock ticks since the start of the program, in hundredths of a second
static reg_t sys_clock(processor_t* p)
{
  return elapsed(p) / (sys_tickfreq() / 100);
}

// RISC-V semihosting entry point
// Parameters:
//   xlen - the XLEN of the target (32 or 64)
//   a0 - the semihosting operation code
//   a1 - the pointer to the semihosting operation arguments
// Return value: depends on the semihosting operation
reg_t processor_t::semihosting(const int xlen, reg_t a0, reg_t a1)
{
  processor_t* p = this;
  D(fprintf(stderr, "semihosting: xlen = %d, a0 = %" PRIx64 ", a1 = %" PRIx64 "\n", xlen, a0, a1));

  switch (a0) {
    case SYS_OPEN:
      return sys_open(p, xlen, a1);
    case SYS_CLOSE:
      return sys_close(p, xlen, a1);
    case SYS_WRITEC:
      return sys_writec(p, a1);
    case SYS_WRITE0:
      return sys_write0(p, a1);
    case SYS_WRITE:
      return sys_write(p, xlen, a1);
    case SYS_READ:
      return sys_read(p, xlen, a1);
    case SYS_READC:
      return sys_readc();
    case SYS_ISERROR:
      return sys_iserror(p, xlen, a1);
    case SYS_ISTTY:
      return sys_isatty(p, xlen, a1);
    case SYS_SEEK:
      return sys_seek(p, xlen, a1);
    case SYS_FLEN:
      return sys_flen(p, xlen, a1);
    case SYS_TMPNAM:
      return sys_tmpnam(p, xlen, a1);
    case SYS_REMOVE:
      return sys_remove(p, xlen, a1);
    case SYS_RENAME:
      return sys_rename(p, xlen, a1);
    case SYS_CLOCK:
      return sys_clock(p);
    case SYS_TIME:
      return sys_time();
    case SYS_SYSTEM:
      return sys_system(p, xlen, a1);
    case SYS_ERRNO:
      return sys_errno();
    case SYS_GET_CMDLINE:
      return sys_get_cmdline(p, xlen, a1, sim->get_target_args());
    case SYS_EXIT:
      sim->sim_exit(sys_exit(p, xlen, a1));
      return 0;
    case SYS_EXIT_EXTENDED:
      sim->sim_exit(sys_exit_extended(p, xlen, a1));
      return 0;
    case SYS_ELAPSED:
      return sys_elapsed(p, xlen, a1);
    case SYS_TICKFREQ:
      return sys_tickfreq();
    case SYS_HEAPINFO:
      fprintf(stderr, "Unimplemented semihosting operation: %" PRIx64 "\n", a0);
      errno = ENOSYS;
      return -1;
    default:
      fprintf(stderr, "Unknown semihosting operation: %" PRIx64 "\n", a0);
      errno = ENOSYS;
      return -1;
  }
}
