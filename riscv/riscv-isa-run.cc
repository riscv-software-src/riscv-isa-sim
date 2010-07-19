#include <unistd.h>
#include "common.h"
#include "sim.h"

int main(int argc, char** argv)
{
  sim_t s(1,MEMSIZE);

  bool debug = false;

  for(int c; (c = getopt(argc,argv,"-d")) != -1; )
  {
    switch(c)
    {
      case '\1':
        s.load_elf(optarg);
        break;
      case 'd':
        debug = true;
        break;
      case '?':
        demand(0,"unrecognized option %c",optopt);
    }
  }

  s.run(debug);
}
