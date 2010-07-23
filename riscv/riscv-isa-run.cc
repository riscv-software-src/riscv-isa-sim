#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "sim.h"
#include "applink.h"

int main(int argc, char** argv)
{
  bool debug = false;
  int nprocs = 1;
  int fromhost_fd = -1, tohost_fd = -1;

  for(int c; (c = getopt(argc,argv,"dpf:t:")) != -1; )
  {
    switch(c)
    {
      case 'd':
        debug = true;
        break;
      case 'p':
        nprocs = atoi(optarg);
        break;
      case 'f':
        fromhost_fd = atoi(optarg);
        break;
      case 't':
        tohost_fd = atoi(optarg);
        break;
    }
  }

  demand(fcntl(fromhost_fd,F_GETFD) >= 0, "fromhost file not open");
  demand(fcntl(tohost_fd,F_GETFD) >= 0, "tohost file not open");

  appserver_link_t applink(tohost_fd,fromhost_fd);

  sim_t s(nprocs,MEMSIZE,&applink);
  s.run(debug);
}
