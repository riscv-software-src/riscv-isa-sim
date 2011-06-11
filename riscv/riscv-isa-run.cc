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
  size_t icsim_sets = 1024, icsim_linesz = 32, icsim_ways = 1;

  for(int c; (c = getopt(argc,argv,"dp:f:t:i:")) != -1; )
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
      case 'i':
        switch(optarg[0])
        {
          case 's':
            icsim_sets = atoi(optarg+1);
            break;
          case 'l':
            icsim_linesz = atoi(optarg+1);
            break;
          case 'a':
            icsim_ways = atoi(optarg+1);
            break;
        }
        break;
    }
  }

  demand(fcntl(fromhost_fd,F_GETFD) >= 0, "fromhost file not open");
  demand(fcntl(tohost_fd,F_GETFD) >= 0, "tohost file not open");

  icsim_t icache(icsim_sets, icsim_ways, icsim_linesz, "I$");
  icsim_t dcache(512, 2, 32, "D$");

  appserver_link_t applink(tohost_fd, fromhost_fd);

  sim_t s(nprocs, &applink, &icache, &dcache);
  try
  {
    s.run(debug);
  }
  catch(quit_sim&)
  {
  }
}
