// See LICENSE for license details.

#include "extension.h"
#include <string>
#include <map>
#include <dlfcn.h>

static std::map<std::string, std::function<extension_t*()>>& extensions()
{
  static std::map<std::string, std::function<extension_t*()>> v;
  return v;
}

void register_extension(const char* name, std::function<extension_t*()> f)
{
  extensions()[name] = f;
}

std::function<extension_t*()> find_extension(const char* name)
{
  if (!extensions().count(name)) {
    // try to find extension xyz by loading libxyz.so
    std::string libname = std::string("lib") + name + ".so";
    if (!dlopen(libname.c_str(), RTLD_LAZY)) {
      fprintf(stderr, "couldn't find extension '%s' (or library '%s')\n",
              name, libname.c_str());
      exit(-1);
    }
    if (!extensions().count(name)) {
      fprintf(stderr, "couldn't find extension '%s' in shared library '%s'\n",
              name, libname.c_str());
      exit(-1);
    }
  }

  return extensions()[name];
}
