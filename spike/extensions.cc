#include "extension.h"
#include "hwacha.h"
#include "dummy-rocc.h"

REGISTER_EXTENSION(dummy, []() { return new dummy_rocc_t; })
REGISTER_EXTENSION(hwacha, []() { return new hwacha_t; })

// Static constructors want to make use of the extensions map, so we
// access it through a function call to guarantee initialization order.
std::map<std::string, std::function<extension_t*()>>& extensions()
{
  static std::map<std::string, std::function<extension_t*()>> v;
  return v;
}
