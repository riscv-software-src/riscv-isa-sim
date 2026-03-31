// See LICENSE for license details.

#include "config.h"
#include "syscall.h"
#include "htif.h"
#include "elfloader.h"
#include "byteorder.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <utility>
using namespace std::placeholders;

#define RISCV_AT_FDCWD -100
#define LEGACY_PK_MAX_FILES 128

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace {

std::string hex_encode(const std::string& input)
{
  static const char digits[] = "0123456789abcdef";
  std::string encoded;
  encoded.reserve(input.size() * 2);

  for (unsigned char ch : input) {
    encoded.push_back(digits[ch >> 4]);
    encoded.push_back(digits[ch & 0xf]);
  }

  return encoded;
}

std::string hex_decode(const std::string& input)
{
  if (input.size() % 2 != 0)
    throw std::runtime_error("hex string has odd length");

  auto hex_value = [](char ch) -> int {
    if (ch >= '0' && ch <= '9')
      return ch - '0';
    if (ch >= 'a' && ch <= 'f')
      return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
      return ch - 'A' + 10;
    return -1;
  };

  std::string decoded;
  decoded.reserve(input.size() / 2);
  for (size_t i = 0; i < input.size(); i += 2) {
    int hi = hex_value(input[i]);
    int lo = hex_value(input[i + 1]);
    if (hi < 0 || lo < 0)
      throw std::runtime_error("hex string contains non-hex character");
    decoded.push_back(char((hi << 4) | lo));
  }

  return decoded;
}

std::string get_fd_path(int fd)
{
  char proc_path[64];
  snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);

  std::vector<char> path(PATH_MAX + 1);
  ssize_t len = readlink(proc_path, path.data(), path.size() - 1);
  if (len < 0)
    return {};

  path[len] = '\0';
  return std::string(path.data(), len);
}

bool is_restorable_host_path(const std::string& path)
{
  const std::string deleted_suffix = " (deleted)";
  return !path.empty() &&
         path.front() == '/' &&
         !(path.size() >= deleted_suffix.size() &&
           path.compare(path.size() - deleted_suffix.size(),
                        deleted_suffix.size(), deleted_suffix) == 0);
}

addr_t lookup_symbol_addr(const std::map<uint64_t, std::string>& addr2symbol,
                          const char* symbol)
{
  for (const auto& entry : addr2symbol)
    if (entry.second == symbol)
      return entry.first;
  return 0;
}

} // namespace

struct riscv_stat
{
  target_endian<uint64_t> dev;
  target_endian<uint64_t> ino;
  target_endian<uint32_t> mode;
  target_endian<uint32_t> nlink;
  target_endian<uint32_t> uid;
  target_endian<uint32_t> gid;
  target_endian<uint64_t> rdev;
  target_endian<uint64_t> __pad1;
  target_endian<uint64_t> size;
  target_endian<uint32_t> blksize;
  target_endian<uint32_t> __pad2;
  target_endian<uint64_t> blocks;
  target_endian<uint64_t> atime;
  target_endian<uint64_t> __pad3;
  target_endian<uint64_t> mtime;
  target_endian<uint64_t> __pad4;
  target_endian<uint64_t> ctime;
  target_endian<uint64_t> __pad5;
  target_endian<uint32_t> __unused4;
  target_endian<uint32_t> __unused5;

  riscv_stat(const struct stat& s, htif_t* htif)
    : dev(htif->to_target<uint64_t>(s.st_dev)),
      ino(htif->to_target<uint64_t>(s.st_ino)),
      mode(htif->to_target<uint32_t>(s.st_mode)),
      nlink(htif->to_target<uint32_t>(s.st_nlink)),
      uid(htif->to_target<uint32_t>(s.st_uid)),
      gid(htif->to_target<uint32_t>(s.st_gid)),
      rdev(htif->to_target<uint64_t>(s.st_rdev)), __pad1(),
      size(htif->to_target<uint64_t>(s.st_size)),
      blksize(htif->to_target<uint32_t>(s.st_blksize)), __pad2(),
      blocks(htif->to_target<uint64_t>(s.st_blocks)),
      atime(htif->to_target<uint64_t>(s.st_atime)), __pad3(),
      mtime(htif->to_target<uint64_t>(s.st_mtime)), __pad4(),
      ctime(htif->to_target<uint64_t>(s.st_ctime)), __pad5(),
      __unused4(), __unused5() {}
};


struct riscv_statx_timestamp {
    target_endian<int64_t>  tv_sec;
    target_endian<uint32_t> tv_nsec;
    target_endian<int32_t>  __reserved;
};

#ifdef HAVE_STATX
struct riscv_statx
{
    target_endian<uint32_t> mask;
    target_endian<uint32_t> blksize;
    target_endian<uint64_t> attributes;
    target_endian<uint32_t> nlink;
    target_endian<uint32_t> uid;
    target_endian<uint32_t> gid;
    target_endian<uint16_t> mode;
    target_endian<uint16_t> __spare0[1];
    target_endian<uint64_t> ino;
    target_endian<uint64_t> size;
    target_endian<uint64_t> blocks;
    target_endian<uint64_t> attributes_mask;
    struct riscv_statx_timestamp atime;
    struct riscv_statx_timestamp btime;
    struct riscv_statx_timestamp ctime;
    struct riscv_statx_timestamp mtime;
    target_endian<uint32_t> rdev_major;
    target_endian<uint32_t> rdev_minor;
    target_endian<uint32_t> dev_major;
    target_endian<uint32_t> dev_minor;
#ifdef HAVE_STATX_MNT_ID
    target_endian<uint64_t> mnt_id;
    target_endian<uint64_t> __spare2;
    target_endian<uint64_t> __spare3[12];
#else
    target_endian<uint64_t> __spare2[14];
#endif

  riscv_statx(const struct statx& s, htif_t* htif)
    : mask(htif->to_target<uint32_t>(s.stx_mask)),
      blksize(htif->to_target<uint32_t>(s.stx_blksize)),
      attributes(htif->to_target<uint64_t>(s.stx_attributes)),
      nlink(htif->to_target<uint32_t>(s.stx_nlink)),
      uid(htif->to_target<uint32_t>(s.stx_uid)),
      gid(htif->to_target<uint32_t>(s.stx_gid)),
      mode(htif->to_target<uint16_t>(s.stx_mode)), __spare0(),
      ino(htif->to_target<uint64_t>(s.stx_ino)),
      size(htif->to_target<uint64_t>(s.stx_size)),
      blocks(htif->to_target<uint64_t>(s.stx_blocks)),
      attributes_mask(htif->to_target<uint64_t>(s.stx_attributes_mask)),
      atime {
        htif->to_target<int64_t>(s.stx_atime.tv_sec),
        htif->to_target<uint32_t>(s.stx_atime.tv_nsec),
        htif->to_target<int32_t>(0)
      },
      btime {
        htif->to_target<int64_t>(s.stx_btime.tv_sec),
        htif->to_target<uint32_t>(s.stx_btime.tv_nsec),
        htif->to_target<int32_t>(0)
      },
      ctime {
        htif->to_target<int64_t>(s.stx_ctime.tv_sec),
        htif->to_target<uint32_t>(s.stx_ctime.tv_nsec),
        htif->to_target<int32_t>(0)
      },
      mtime {
        htif->to_target<int64_t>(s.stx_mtime.tv_sec),
        htif->to_target<uint32_t>(s.stx_mtime.tv_nsec),
        htif->to_target<int32_t>(0)
      },
      rdev_major(htif->to_target<uint32_t>(s.stx_rdev_major)),
      rdev_minor(htif->to_target<uint32_t>(s.stx_rdev_minor)),
      dev_major(htif->to_target<uint32_t>(s.stx_dev_major)),
      dev_minor(htif->to_target<uint32_t>(s.stx_dev_minor)),
#ifdef HAVE_STATX_MNT_ID
      mnt_id(htif->to_target<uint64_t>(s.stx_mnt_id)),
      __spare2(), __spare3()
#else
      __spare2()
#endif
      {}
};
#endif

syscall_t::syscall_t(htif_t* htif)
  : htif(htif), memif(&htif->memif()), table(2048)
{
  table[17] = &syscall_t::sys_getcwd;
  table[25] = &syscall_t::sys_fcntl;
  table[34] = &syscall_t::sys_mkdirat;
  table[35] = &syscall_t::sys_unlinkat;
  table[37] = &syscall_t::sys_linkat;
  table[38] = &syscall_t::sys_renameat;
  table[46] = &syscall_t::sys_ftruncate;
  table[48] = &syscall_t::sys_faccessat;
  table[49] = &syscall_t::sys_chdir;
  table[56] = &syscall_t::sys_openat;
  table[57] = &syscall_t::sys_close;
  table[62] = &syscall_t::sys_lseek;
  table[63] = &syscall_t::sys_read;
  table[64] = &syscall_t::sys_write;
  table[67] = &syscall_t::sys_pread;
  table[68] = &syscall_t::sys_pwrite;
  table[78] = &syscall_t::sys_readlinkat;
  table[79] = &syscall_t::sys_fstatat;
  table[80] = &syscall_t::sys_fstat;
  table[93] = &syscall_t::sys_exit;
  table[291] = &syscall_t::sys_statx;
  table[1039] = &syscall_t::sys_lstat;
  table[2011] = &syscall_t::sys_getmainvars;

  register_command(0, std::bind(&syscall_t::handle_syscall, this, _1), "syscall");

  int stdin_fd = dup(0), stdout_fd0 = dup(1), stdout_fd1 = dup(1);
  if (stdin_fd < 0 || stdout_fd0 < 0 || stdout_fd1 < 0)
    throw std::runtime_error("could not dup stdin/stdout");

  fds.alloc_stdio(stdin_fd, 0); // stdin -> stdin
  fds.alloc_stdio(stdout_fd0, 1); // stdout -> stdout
  fds.alloc_stdio(stdout_fd1, 1); // stderr -> stdout
}

syscall_t::~syscall_t() {
  fds.close_all();
}

void syscall_t::save_checkpoint_state(std::ostream& out) const
{
  out << "# spike syscall host state" << std::endl;

  char* cwd = getcwd(nullptr, 0);
  if (cwd != nullptr) {
    out << "cwd " << hex_encode(cwd) << std::endl;
    free(cwd);
  }

  const auto& entries = fds.entries();
  for (size_t guest_fd = 0; guest_fd < entries.size(); ++guest_fd) {
    const auto& entry = entries[guest_fd];
    if (entry.host_fd < 0)
      continue;

    if (entry.kind == fds_t::fd_kind_t::stdio) {
      out << "fd " << guest_fd << " stdio " << entry.stdio_fd << std::endl;
      continue;
    }

    const std::string path = get_fd_path(entry.host_fd);
    if (!is_restorable_host_path(path)) {
      out << "fd " << guest_fd << " opaque" << std::endl;
      continue;
    }

    int flags = fcntl(entry.host_fd, F_GETFL);
    if (flags < 0) {
      out << "fd " << guest_fd << " opaque" << std::endl;
      continue;
    }

    errno = 0;
    off_t offset = lseek(entry.host_fd, 0, SEEK_CUR);
    long long saved_offset = offset >= 0 ? static_cast<long long>(offset) : -1;

    out << "fd " << guest_fd << " path " << flags << " "
        << saved_offset << " " << hex_encode(path) << std::endl;
  }
}

void syscall_t::load_checkpoint_state(std::istream& in)
{
  fds.close_all();

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string tag;
    iss >> tag;

    if (tag == "cwd") {
      std::string encoded_cwd;
      if (!(iss >> encoded_cwd))
        throw std::runtime_error("malformed checkpoint host cwd entry");

      const std::string cwd = hex_decode(encoded_cwd);
      if (chdir(cwd.c_str()) != 0)
        throw std::runtime_error("failed to restore checkpoint cwd: " + cwd);
      continue;
    }

    if (tag != "fd")
      throw std::runtime_error("unknown checkpoint host-state record: " + tag);

    reg_t guest_fd;
    std::string type;
    if (!(iss >> guest_fd >> type))
      throw std::runtime_error("malformed checkpoint fd entry");

    if (type == "stdio") {
      int stdio_fd;
      if (!(iss >> stdio_fd))
        throw std::runtime_error("malformed stdio checkpoint fd entry");

      int host_fd = dup(stdio_fd);
      if (host_fd < 0)
        throw std::runtime_error("failed to dup stdio while restoring checkpoint state");

      fds_t::fd_entry_t entry;
      entry.host_fd = host_fd;
      entry.kind = fds_t::fd_kind_t::stdio;
      entry.stdio_fd = stdio_fd;
      fds.alloc_at(guest_fd, entry);
      continue;
    }

    if (type == "path") {
      int flags;
      long long saved_offset;
      std::string encoded_path;
      if (!(iss >> flags >> saved_offset >> encoded_path))
        throw std::runtime_error("malformed path checkpoint fd entry");

      const std::string path = hex_decode(encoded_path);
      int host_fd = open(path.c_str(), flags);
      if (host_fd < 0)
        throw std::runtime_error("failed to reopen checkpoint fd path: " + path);

      if (saved_offset >= 0 && lseek(host_fd, static_cast<off_t>(saved_offset), SEEK_SET) < 0) {
        close(host_fd);
        throw std::runtime_error("failed to restore checkpoint fd offset for: " + path);
      }

      fds_t::fd_entry_t entry;
      entry.host_fd = host_fd;
      fds.alloc_at(guest_fd, entry);
      continue;
    }

    if (type == "opaque") {
      std::cerr << "warning: checkpoint fd " << guest_fd
                << " is not restorable; leaving it closed" << std::endl;
      continue;
    }

    throw std::runtime_error("unknown checkpoint fd type: " + type);
  }
}

bool syscall_t::restore_legacy_pk_checkpoint_state()
{
  if (htif->targs.size() < 2)
    return false;

  class nop_memif_t : public memif_t {
   public:
    nop_memif_t(htif_t* htif) : memif_t(htif) {}
    void read(addr_t, size_t, void*) override {}
    void write(addr_t, size_t, const void*) override {}
  } nop_memif(htif);

  addr_t files_addr = 0;
  reg_t dummy_entry = 0;
  try {
    auto symbols = load_elf(htif->targs[0].c_str(), &nop_memif, &dummy_entry,
                            0, htif->expected_xlen);
    auto it = symbols.find("files");
    if (it != symbols.end())
      files_addr = it->second;
  } catch (const std::exception&) {
  }

  if (files_addr == 0)
    files_addr = lookup_symbol_addr(htif->addr2symbol, "files");

  if (files_addr == 0)
    return false;

  struct serialized_file_t {
    target_endian<int32_t> kfd;
    target_endian<uint32_t> refcnt;
  };

  std::vector<serialized_file_t> files(LEGACY_PK_MAX_FILES);
  memif->read(files_addr, files.size() * sizeof(files[0]), files.data());

  std::vector<int32_t> active_guest_kfds;
  for (const auto& file : files) {
    const int32_t guest_kfd = htif->from_target(file.kfd);
    const uint32_t refcnt = htif->from_target(file.refcnt);
    if (refcnt == 0 || guest_kfd < 0 || guest_kfd <= 2)
      continue;

    bool seen = false;
    for (int32_t existing : active_guest_kfds) {
      if (existing == guest_kfd) {
        seen = true;
        break;
      }
    }
    if (!seen)
      active_guest_kfds.push_back(guest_kfd);
  }

  if (active_guest_kfds.empty())
    return false;

  if (active_guest_kfds.size() != 1) {
    std::cerr << "warning: checkpoint is missing hoststate and has "
              << active_guest_kfds.size()
              << " active guest-backed files; legacy restore cannot reconstruct them"
              << std::endl;
    return false;
  }

  const std::string payload_path = htif->targs[1];
  int host_fd = open(payload_path.c_str(), O_RDONLY);
  if (host_fd < 0) {
    std::cerr << "warning: failed to reopen payload for legacy checkpoint restore: "
              << payload_path << std::endl;
    return false;
  }

  fds_t::fd_entry_t entry;
  entry.host_fd = host_fd;
  fds.alloc_at(active_guest_kfds.front(), entry);

  std::cerr << "warning: checkpoint missing .hoststate; restored legacy pk payload fd "
            << active_guest_kfds.front() << " from " << payload_path << std::endl;
  return true;
}

std::string syscall_t::do_chroot(const char* fn)
{
  if (!chroot.empty() && *fn == '/')
    return chroot + fn;
  return fn;
}

std::string syscall_t::undo_chroot(const char* fn)
{
  if (chroot.empty())
    return fn;
  if (strncmp(fn, chroot.c_str(), chroot.size()) == 0
      && (chroot.back() == '/' || fn[chroot.size()] == '/'))
    return fn + chroot.size() - (chroot.back() == '/');
  return "/";
}

void syscall_t::handle_syscall(command_t cmd)
{
  if (cmd.payload() & 1) // test pass/fail
  {
    htif->exitcode = cmd.payload();
    if (htif->exit_code())
      std::cerr << "*** FAILED *** (tohost = " << htif->exit_code() << ")" << std::endl;
    return;
  }
  else // proxied system call
    dispatch(cmd.payload());

  cmd.respond(1);
}

reg_t syscall_t::sys_exit(reg_t code, reg_t a1, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  htif->htif_exit(code << 1 | 1);
  return 0;
}

static reg_t sysret_errno(sreg_t ret)
{
  return ret == -1 ? -errno : ret;
}

reg_t syscall_t::sys_read(reg_t fd, reg_t pbuf, reg_t len, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> buf(len);
  ssize_t ret = read(fds.lookup(fd), buf.data(), len);
  reg_t ret_errno = sysret_errno(ret);
  if (ret > 0)
    memif->write(pbuf, ret, buf.data());
  return ret_errno;
}

reg_t syscall_t::sys_pread(reg_t fd, reg_t pbuf, reg_t len, reg_t off, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> buf(len);
  ssize_t ret = pread(fds.lookup(fd), buf.data(), len, off);
  reg_t ret_errno = sysret_errno(ret);
  if (ret > 0)
    memif->write(pbuf, ret, buf.data());
  return ret_errno;
}

reg_t syscall_t::sys_write(reg_t fd, reg_t pbuf, reg_t len, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> buf(len);
  memif->read(pbuf, len, buf.data());
  reg_t ret = sysret_errno(write(fds.lookup(fd), buf.data(), len));
  return ret;
}

reg_t syscall_t::sys_pwrite(reg_t fd, reg_t pbuf, reg_t len, reg_t off, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> buf(len);
  memif->read(pbuf, len, buf.data());
  reg_t ret = sysret_errno(pwrite(fds.lookup(fd), buf.data(), len, off));
  return ret;
}

reg_t syscall_t::sys_close(reg_t fd, reg_t a1, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  if (close(fds.lookup(fd)) < 0)
    return sysret_errno(-1);
  fds.dealloc(fd);
  return 0;
}

reg_t syscall_t::sys_lseek(reg_t fd, reg_t ptr, reg_t dir, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  return sysret_errno(lseek(fds.lookup(fd), ptr, dir));
}

reg_t syscall_t::sys_fstat(reg_t fd, reg_t pbuf, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  struct stat buf;
  reg_t ret = sysret_errno(fstat(fds.lookup(fd), &buf));
  if (ret != (reg_t)-1)
  {
    riscv_stat rbuf(buf, htif);
    memif->write(pbuf, sizeof(rbuf), &rbuf);
  }
  return ret;
}

reg_t syscall_t::sys_fcntl(reg_t fd, reg_t cmd, reg_t arg, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  return sysret_errno(fcntl(fds.lookup(fd), cmd, arg));
}

reg_t syscall_t::sys_ftruncate(reg_t fd, reg_t len, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  return sysret_errno(ftruncate(fds.lookup(fd), len));
}

reg_t syscall_t::sys_lstat(reg_t pname, reg_t len, reg_t pbuf, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> name(len);
  memif->read(pname, len, name.data());

  struct stat buf;
  reg_t ret = sysret_errno(lstat(do_chroot(name.data()).c_str(), &buf));
  if (ret != (reg_t)-1)
  {
    riscv_stat rbuf(buf, htif);
    memif->write(pbuf, sizeof(rbuf), &rbuf);
  }
  return ret;
}

reg_t syscall_t::sys_statx(reg_t fd, reg_t pname, reg_t len, reg_t flags, reg_t mask, reg_t pbuf, reg_t a6)
{
#ifndef HAVE_STATX
  return -ENOSYS;
#else
  std::vector<char> name(len);
  memif->read(pname, len, name.data());

  struct statx buf;
  reg_t ret = sysret_errno(statx(fds.lookup(fd), do_chroot(name.data()).c_str(), flags, mask, &buf));
  if (ret != (reg_t)-1)
  {
    riscv_statx rbuf(buf, htif);
    memif->write(pbuf, sizeof(rbuf), &rbuf);
  }
  return ret;
#endif
}

#define AT_SYSCALL(syscall, fd, name, ...) \
  (syscall(fds.lookup(fd), int(fd) == RISCV_AT_FDCWD ? do_chroot(name).c_str() : (name), __VA_ARGS__))

reg_t syscall_t::sys_openat(reg_t dirfd, reg_t pname, reg_t len, reg_t flags, reg_t mode, reg_t a5, reg_t a6)
{
  std::vector<char> name(len);
  memif->read(pname, len, name.data());
  int fd = sysret_errno(AT_SYSCALL(openat, dirfd, name.data(), flags, mode));
  if (fd < 0)
    return sysret_errno(-1);
  return fds.alloc(fd);
}

reg_t syscall_t::sys_fstatat(reg_t dirfd, reg_t pname, reg_t len, reg_t pbuf, reg_t flags, reg_t a5, reg_t a6)
{
  std::vector<char> name(len);
  memif->read(pname, len, name.data());

  struct stat buf;
  reg_t ret = sysret_errno(AT_SYSCALL(fstatat, dirfd, name.data(), &buf, flags));
  if (ret != (reg_t)-1)
  {
    riscv_stat rbuf(buf, htif);
    memif->write(pbuf, sizeof(rbuf), &rbuf);
  }
  return ret;
}

reg_t syscall_t::sys_faccessat(reg_t dirfd, reg_t pname, reg_t len, reg_t mode, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> name(len);
  memif->read(pname, len, name.data());
  return sysret_errno(AT_SYSCALL(faccessat, dirfd, name.data(), mode, 0));
}

reg_t syscall_t::sys_renameat(reg_t odirfd, reg_t popath, reg_t olen, reg_t ndirfd, reg_t pnpath, reg_t nlen, reg_t a6)
{
  std::vector<char> opath(olen), npath(nlen);
  memif->read(popath, olen, opath.data());
  memif->read(pnpath, nlen, npath.data());
  return sysret_errno(renameat(fds.lookup(odirfd), int(odirfd) == RISCV_AT_FDCWD ? do_chroot(opath.data()).c_str() : opath.data(),
                             fds.lookup(ndirfd), int(ndirfd) == RISCV_AT_FDCWD ? do_chroot(npath.data()).c_str() : npath.data()));
}

reg_t syscall_t::sys_linkat(reg_t odirfd, reg_t poname, reg_t olen, reg_t ndirfd, reg_t pnname, reg_t nlen, reg_t flags)
{
  std::vector<char> oname(olen), nname(nlen);
  memif->read(poname, olen, oname.data());
  memif->read(pnname, nlen, nname.data());
  return sysret_errno(linkat(fds.lookup(odirfd), int(odirfd) == RISCV_AT_FDCWD ? do_chroot(oname.data()).c_str() : oname.data(),
                             fds.lookup(ndirfd), int(ndirfd) == RISCV_AT_FDCWD ? do_chroot(nname.data()).c_str() : nname.data(),
                             flags));
}

reg_t syscall_t::sys_unlinkat(reg_t dirfd, reg_t pname, reg_t len, reg_t flags, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> name(len);
  memif->read(pname, len, name.data());
  return sysret_errno(AT_SYSCALL(unlinkat, dirfd, name.data(), flags));
}

reg_t syscall_t::sys_mkdirat(reg_t dirfd, reg_t pname, reg_t len, reg_t mode, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> name(len);
  memif->read(pname, len, name.data());
  return sysret_errno(AT_SYSCALL(mkdirat, dirfd, name.data(), mode));
}

reg_t syscall_t::sys_getcwd(reg_t pbuf, reg_t size, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<char> buf(size);
  char* ret = getcwd(buf.data(), size);
  if (ret == NULL)
    return sysret_errno(-1);
  std::string tmp = undo_chroot(buf.data());
  if (size <= tmp.size())
    return -ENOMEM;
  memif->write(pbuf, tmp.size() + 1, tmp.data());
  return tmp.size() + 1;
}

reg_t syscall_t::sys_getmainvars(reg_t pbuf, reg_t limit, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  std::vector<std::string> args = htif->target_args();
  std::vector<target_endian<uint64_t>> words(args.size() + 3);
  words[0] = htif->to_target<uint64_t>(args.size());
  words[args.size()+1] = target_endian<uint64_t>::zero; // argv[argc] = NULL
  words[args.size()+2] = target_endian<uint64_t>::zero; // envp[0] = NULL

  size_t sz = (args.size() + 3) * sizeof(words[0]);
  for (size_t i = 0; i < args.size(); i++)
  {
    words[i+1] = htif->to_target<uint64_t>(sz + pbuf);
    sz += args[i].length() + 1;
  }

  std::vector<char> bytes(sz);
  memcpy(bytes.data(), words.data(), sizeof(words[0]) * words.size());
  for (size_t i = 0; i < args.size(); i++)
    strcpy(&bytes[htif->from_target(words[i+1]) - pbuf], args[i].c_str());

  if (bytes.size() > limit)
    return -ENOMEM;

  memif->write(pbuf, bytes.size(), bytes.data());
  return 0;
}

reg_t syscall_t::sys_chdir(reg_t path, reg_t a1, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6)
{
  size_t size = 0;
  while (memif->read_uint8(path + size++))
    ;
  std::vector<char> buf(size);
  for (size_t offset = 0;; offset++)
  {
    buf[offset] = memif->read_uint8(path + offset);
    if (!buf[offset])
      break;
  }
  return sysret_errno(chdir(buf.data()));
}

void syscall_t::dispatch(reg_t mm)
{
  target_endian<reg_t> magicmem[8];
  memif->read(mm, sizeof(magicmem), magicmem);

  reg_t n = htif->from_target(magicmem[0]);
  if (n >= table.size() || !table[n])
    throw std::runtime_error("bad syscall #" + std::to_string(n));

  magicmem[0] = htif->to_target((this->*table[n])(htif->from_target(magicmem[1]), htif->from_target(magicmem[2]), htif->from_target(magicmem[3]), htif->from_target(magicmem[4]), htif->from_target(magicmem[5]), htif->from_target(magicmem[6]), htif->from_target(magicmem[7])));

  memif->write(mm, sizeof(magicmem), magicmem);
}

reg_t fds_t::alloc_entry(fd_entry_t entry)
{
  reg_t i;
  for (i = 0; i < fds.size(); i++)
    if (fds[i].host_fd == -1)
      break;

  if (i == fds.size())
    fds.resize(i + 1);

  fds[i] = std::move(entry);
  return i;
}

void fds_t::ensure_size(reg_t fd)
{
  if (fd >= fds.size())
    fds.resize(fd + 1);
}

reg_t fds_t::alloc(int fd)
{
  fd_entry_t entry;
  entry.host_fd = fd;
  return alloc_entry(std::move(entry));
}

reg_t fds_t::alloc_stdio(int fd, int stdio_fd)
{
  fd_entry_t entry;
  entry.host_fd = fd;
  entry.kind = fd_kind_t::stdio;
  entry.stdio_fd = stdio_fd;
  return alloc_entry(std::move(entry));
}

void fds_t::alloc_at(reg_t fd, fd_entry_t entry)
{
  ensure_size(fd);
  if (fds[fd].host_fd >= 0)
    close(fds[fd].host_fd);
  fds[fd] = std::move(entry);
}

void fds_t::close_all()
{
  for (auto& entry : fds) {
    if (entry.host_fd >= 0)
      close(entry.host_fd);
    entry = fd_entry_t();
  }
  fds.clear();
}

void fds_t::dealloc(reg_t fd)
{
  if (fd >= fds.size())
    return;
  fds[fd] = fd_entry_t();
}

int fds_t::lookup(reg_t fd) const
{
  if (int(fd) == RISCV_AT_FDCWD)
    return AT_FDCWD;
  return fd >= fds.size() ? -1 : fds[fd].host_fd;
}

void syscall_t::set_chroot(const char* where)
{
  char buf1[PATH_MAX], buf2[PATH_MAX];

  if (getcwd(buf1, sizeof(buf1)) == NULL
      || chdir(where) != 0
      || getcwd(buf2, sizeof(buf2)) == NULL
      || chdir(buf1) != 0)
  {
    fprintf(stderr, "could not chroot to %s\n", where);
    exit(-1);
  }

  chroot = buf2;
}

reg_t syscall_t::sys_readlinkat(reg_t dirfd, reg_t ppathname, reg_t ppathname_size,
                                reg_t pbuf, reg_t bufsiz, reg_t a5, reg_t a6)
{
  std::vector<char> pathname(ppathname_size);
  memif->read(ppathname, ppathname_size, pathname.data());

  std::vector<char> buf(bufsiz);
  ssize_t ret = sysret_errno(AT_SYSCALL(readlinkat, dirfd, pathname.data(), buf.data(), bufsiz));
  if (ret > 0)
    memif->write(pbuf, ret, buf.data());
  return ret;
}
