#ifndef _TERM_H
#define _TERM_H

class canonical_terminal_t
{
 public:
  static int read();
  static void write(char);
};

#endif
