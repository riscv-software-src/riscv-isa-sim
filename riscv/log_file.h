// See LICENSE for license details.
#ifndef _RISCV_LOGFILE_H
#define _RISCV_LOGFILE_H

#include <stdio.h>
#include <memory>
#include <sstream>
#include <stdexcept>

// Header-only class wrapping a log file. When constructed with an
// actual path, it opens the named file for writing. When constructed
// with the null path, it wraps stderr.
class log_file_t
{
public:
  log_file_t(const char *path)
    : wrapped_file (nullptr, &fclose)
  {
    if (!path)
      return;

    wrapped_file.reset(fopen(path, "w"));
    if (! wrapped_file) {
      std::ostringstream oss;
      oss << "Failed to open log file at `" << path << "': "
          << strerror (errno);
      throw std::runtime_error(oss.str());
    }
  }

  FILE *get() { return wrapped_file ? wrapped_file.get() : stderr; }

private:
  std::unique_ptr<FILE, decltype(&fclose)> wrapped_file;
};

#endif
