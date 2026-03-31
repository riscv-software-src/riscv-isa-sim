// See LICENSE for license details.

#ifndef __SYSCALL_H
#define __SYSCALL_H

#include "device.h"
#include "memif.h"
#include <iosfwd>
#include <vector>
#include <string>

class syscall_t;
typedef reg_t (syscall_t::*syscall_func_t)(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);

class htif_t;
class memif_t;

class fds_t
{
 public:
  enum class fd_kind_t {
    generic,
    stdio,
  };

  struct fd_entry_t {
    int host_fd = -1;
    fd_kind_t kind = fd_kind_t::generic;
    int stdio_fd = -1;
  };

  reg_t alloc(int fd);
  reg_t alloc_stdio(int fd, int stdio_fd);
  void alloc_at(reg_t fd, fd_entry_t entry);
  void close_all();
  void dealloc(reg_t fd);
  int lookup(reg_t fd) const;
  const std::vector<fd_entry_t>& entries() const { return fds; }
 private:
  reg_t alloc_entry(fd_entry_t entry);
  void ensure_size(reg_t fd);

  std::vector<fd_entry_t> fds;
};

class syscall_t : public device_t
{
 public:
  syscall_t(htif_t*);
  ~syscall_t();

  void set_chroot(const char* where);
  void save_checkpoint_state(std::ostream& out) const;
  void load_checkpoint_state(std::istream& in);
  bool restore_legacy_pk_checkpoint_state();
  
 private:
  const char* identity() { return "syscall_proxy"; }

  htif_t* htif;
  memif_t* memif;
  std::vector<syscall_func_t> table;
  fds_t fds;

  void handle_syscall(command_t cmd);
  void dispatch(addr_t mm);

  std::string chroot;
  std::string do_chroot(const char* fn);
  std::string undo_chroot(const char* fn);

  reg_t sys_exit(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_openat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_read(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_pread(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_write(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_pwrite(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_close(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_lseek(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_fstat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_lstat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_statx(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_fstatat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_faccessat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_fcntl(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_ftruncate(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_renameat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_linkat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_unlinkat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_mkdirat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_getcwd(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_getmainvars(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_chdir(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
  reg_t sys_readlinkat(reg_t, reg_t, reg_t, reg_t, reg_t, reg_t, reg_t);
};

#endif
