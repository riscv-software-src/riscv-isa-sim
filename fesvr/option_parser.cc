// See LICENSE for license details.

#include "option_parser.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

void option_parser_t::option(char c, const char* s, int arg, std::function<void(const char*)> action)
{
  opts.push_back(option_t(c, s, arg, action));
}

const char* const* option_parser_t::parse(const char* const* argv0)
{
  assert(argv0);
  const char* const* argv = argv0 + 1;
  for (const char* opt; (opt = *argv) != NULL && opt[0] == '-'; argv++)
  {
    bool found = false;
    for (auto it = opts.begin(); !found && it != opts.end(); it++)
    {
      size_t slen = it->str ? strlen(it->str) : 0;
      bool chr_match = opt[1] != '-' && it->chr && opt[1] == it->chr;
      bool str_match = opt[1] == '-' && slen && strncmp(opt+2, it->str, slen) == 0;
      if (chr_match || (str_match && (opt[2+slen] == '=' || opt[2+slen] == '\0')))
      {
        const char* optarg =
          chr_match ? (opt[2] ? &opt[2] : NULL) :
          opt[2+slen] ? &opt[3+slen] :
          it->arg ? *(++argv) : NULL;
        if (optarg && !it->arg)
          error("no argument allowed for option", *argv0, opt);
        if (!optarg && it->arg)
          error("argument required for option", *argv0, opt);
        it->func(optarg);
        found = true;
      }
    }
    if (!found)
      error("unrecognized option", *argv0, opt);
  }
  return argv;
}

void option_parser_t::error(const char* msg, const char* argv0, const char* arg)
{
  fprintf(stderr, "%s: %s %s\n", argv0, msg, arg ? arg : "");
  if (helpmsg) helpmsg();
  exit(1);
}
