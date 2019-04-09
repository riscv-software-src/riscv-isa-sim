// See LICENSE for license details.

#ifndef _OPTION_PARSER_H
#define _OPTION_PARSER_H

#include <vector>
#include <functional>

class option_parser_t
{
 public:
  option_parser_t() : helpmsg(0) {}
  void help(void (*helpm)(void)) { helpmsg = helpm; }
  void option(char c, const char* s, int arg, std::function<void(const char*)> action);
  const char* const* parse(const char* const* argv0);
 private:
  struct option_t
  {
    char chr;
    const char* str;
    int arg;
    std::function<void(const char*)> func;
    option_t(char chr, const char* str, int arg, std::function<void(const char*)> func)
     : chr(chr), str(str), arg(arg), func(func) {}
  };
  std::vector<option_t> opts;
  void (*helpmsg)(void);
  void error(const char* msg, const char* argv0, const char* arg);
};

#endif
