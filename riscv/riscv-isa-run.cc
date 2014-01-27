#include "sim.h"
#include "htif.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>

static void help()
{
  fprintf(stderr, "usage: riscv-isa-run [host options] <target program> [target options]\n");
  fprintf(stderr, "Host Options:\n");
  fprintf(stderr, "  -p <n>     Simulate <n> processors\n");
  fprintf(stderr, "  -m <n>     Provide <n> MB of target memory\n");
  fprintf(stderr, "  -d         Interactive debug mode\n");
  fprintf(stderr, "  -h         Print this help message\n");
  exit(1);
}

int main(int argc, char** argv)
{
  bool debug = false;
  int nprocs = 1;
  int mem_mb = 0;

  // parse command-line arguments
  for(int c; (c = getopt(argc,argv,"hdp:m:")) != -1; )
  {
    switch(c)
    {
      case 'd':
        debug = true;
        break;
      case 'p':
        nprocs = atoi(optarg);
        break;
      case 'm':
        mem_mb = atoi(optarg);
        break;
      default:
        fprintf(stderr, "unknown option: -%c", optopt);
      case 'h':
        help();
    }
  }

  if (optind == argc)
    help();

  std::vector<std::string> htif_args(argv + optind, argv + argc);
  sim_t s(nprocs, mem_mb, htif_args);
  s.run(debug);
}
