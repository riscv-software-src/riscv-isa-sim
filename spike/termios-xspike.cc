// See LICENSE for license details.

// termios-xspike sets up a canonical terminal and blocks forever.
// It allows us to send Ctrl-C etc. to the target machine.

#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  struct termios old_tios;
  if (tcgetattr(0, &old_tios) < 0)
    return -1;

  signal(SIGTERM, [](int) { });

  struct termios new_tios = old_tios;
  new_tios.c_lflag &= ~(ICANON | ECHO | ISIG);
  if (tcsetattr(0, TCSANOW, &new_tios) < 0)
    return -1;

  pause();

  return tcsetattr(0, TCSANOW, &old_tios);
}
