// See LICENSE for license details.

// xspike forks an xterm for spike's target machine console,
// preserving the current terminal for debugging.

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdio>
#include <climits>
#include <cstring>
#include <stdexcept>

static pid_t fork_spike(int tty_fd, int argc, char** argv);
static pid_t fork_xterm(int* tty_fd);

int main(int argc, char** argv)
{
  int tty_fd, wait_status, ret = -1;
  pid_t xterm, spike;

  static bool signal_exit = false;
  auto handle_signal = [](int) { signal_exit = true; };

  if ((xterm = fork_xterm(&tty_fd)) < 0)
  {
    fprintf(stderr, "could not open xterm\n");
    goto out;
  }

  signal(SIGINT, handle_signal);

  if ((spike = fork_spike(tty_fd, argc, argv)) < 0)
  {
    fprintf(stderr, "could not open spike\n");
    goto close_xterm;
  }

  while ((ret = waitpid(spike, &wait_status, 0)) < 0)
    if (signal_exit)
      break;

  if (ret < 0) // signal_exit
    kill(spike, SIGTERM);
  else
    ret = WIFEXITED(wait_status) ? WEXITSTATUS(wait_status) : -1;

close_xterm:
  kill(-xterm, SIGTERM);
out:
  return ret;
}

static pid_t fork_spike(int tty_fd, int argc, char** argv)
{
  pid_t pid = fork();
  if (pid < 0)
    return -1;

  if (pid == 0)
  {
    if (dup2(tty_fd, STDIN_FILENO) < 0 || dup2(tty_fd, STDOUT_FILENO) < 0)
      return -1;
    execvp("spike", argv);
    return -1;
  }

  return pid;
}

static pid_t fork_xterm(int* tty_fd)
{
  static const char cmd[] = "3>&1 xterm -title xspike -e sh -c 'tty 1>&3; termios-xspike'";

  int fds[2];
  if (pipe(fds) < 0)
    return -1;

  pid_t pid = fork();
  if (pid < 0)
    return -1;

  if (pid == 0)
  {
    setpgid(0, 0);
    if (dup2(fds[1], STDOUT_FILENO) < 0)
      return -1;
    execl("/bin/sh", "sh", "-c", cmd, NULL);
    return -1;
  }

  char tty[PATH_MAX];
  ssize_t ttylen = read(fds[0], tty, sizeof(tty));
  if (ttylen <= 1 || tty[ttylen-1] != '\n')
    return -1;
  tty[ttylen-1] = '\0';
  if ((*tty_fd = open(tty, O_RDWR)) < 0)
    return -1;

  return pid;
}
