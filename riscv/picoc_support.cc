#include "sim.h"

#define UNIX_HOST
#include <picoc.h>

#define PICOC_HEAP_SIZE 10*1024*1024
extern "C" int last_spike_result;
sim_t* picoc_sim;
char picoc_buffer[128*1024];
int picoc_cond_counter = 0;
char picoc_cond_fn[512];

int picoc_always_counter = 0;
char picoc_always_fn[512];

void sim_t::picoc_init(sim_t* sim) {
  picoc_sim = sim;
  PicocInitialise(PICOC_HEAP_SIZE);
  const char* include_src = "#include<stdio.h> #include<spike.h>";
  PicocParse("nofile", include_src, strlen(include_src), TRUE, TRUE, FALSE);

  const char* always_src = "void always_0() { }";
  sprintf(picoc_always_fn, "always_0();");
  PicocParse("nofile", always_src, strlen(always_src), TRUE, TRUE, FALSE);
}

void sim_t::picoc_shutdown() {
  PicocCleanup();
}

void sim_t::picoc_eval(std::string& cmd) {
  PicocParse("nofile", cmd.c_str(), strlen(cmd.c_str()), TRUE, TRUE, FALSE);
  // TODO: Report errors!
}

void sim_t::picoc_compile_conditional(std::string& cond) {
  sprintf(picoc_buffer, "void cond_%d() { result = (%s); }", picoc_cond_counter, cond.c_str());
  sprintf(picoc_cond_fn, "cond_%d();", picoc_cond_counter);
  PicocParse("nofile", picoc_buffer, strlen(picoc_buffer), TRUE, TRUE, FALSE);
  picoc_cond_counter++;
  // TODO: Report errors!
}

bool sim_t::picoc_eval_conditional(void) {
  PicocParse("nofile", picoc_cond_fn, strlen(picoc_cond_fn), TRUE, TRUE, FALSE);
  return (bool)last_spike_result;
}

void sim_t::picoc_eval_always(void) {
  PicocParse("nofile", picoc_always_fn, strlen(picoc_always_fn), TRUE, TRUE, FALSE);
}

inline int sim_t::picoc_get_pc(int p) {
  if(p >= (int)num_cores()) {
    fprintf(stderr, "Invalid processor id!\n");
    return -1;
  }

  return procs[p]->state.pc;
}

inline int sim_t::picoc_get_reg(int p, int r) {
  if(p >= (int)num_cores() || r >= NXPR) {
    fprintf(stderr, "Invalid register!\n");
    return -1;
  }

  return procs[p]->state.XPR[r];
}

inline int sim_t::picoc_get_mem(char* addr) {
  std::string addr_str(addr);
  std::vector<std::string> args;
  args.push_back(addr);
  return get_mem(args);
}

extern "C" int picoc_get_pc(int p) {
  return picoc_sim->picoc_get_pc(p);
}

extern "C" int picoc_get_reg(int p, int r) {
  return picoc_sim->picoc_get_reg(p, r);
}

extern "C" int picoc_get_mem(char* addr) {
  return picoc_sim->picoc_get_mem(addr);
}

static void join_args(std::string& result, const std::vector<std::string>& args) {
  result.reserve(1024);
  for (auto it = args.begin(); it != args.end(); ++it) {
    result.append(*it);
    result.append(" ");
  }
}

void sim_t::interactive_evalc(const std::string& cmd, const std::vector<std::string>& args)
{
  // Join arguments into one string
  std::string str;
  join_args(str, args);
  str.append(";");

  if (PicocPlatformSetExitPoint()) {
    printf("Error in evalc expression!\n");
    return;
  }

  picoc_eval(str);
  // TODO: Check!
}


void sim_t::interactive_alwaysc(const std::string& cmd, const std::vector<std::string>& args) {
  // Join arguments into one string
  std::string str;
  join_args(str, args);
  str.append(";");

  if (PicocPlatformSetExitPoint()) {
    printf("Error in alwaysc expression!\n");
    return;
  }

  sprintf(picoc_buffer, "void always_%d() { %s }", picoc_always_counter+1, str.c_str());
  sprintf(picoc_always_fn, "always_%d();", picoc_always_counter+1);
  PicocParse("nofile", picoc_buffer, strlen(picoc_buffer), TRUE, TRUE, FALSE);
  picoc_always_counter++;
}


void sim_t::interactive_untilc(const std::string& cmd, const std::vector<std::string>& args)
{
  bool cmd_until = cmd == "untilc";

  // Join arguments into one string
  std::string str;
  join_args(str, args);

  if (PicocPlatformSetExitPoint()) {
    printf("Error in untilc expression!\n");
    return;
  }

  picoc_compile_conditional(str);
  // TODO: Check!

  ctrlc_pressed = false;

  while (1)
  {
    try
    {
      bool result = picoc_eval_conditional();

      if (cmd_until == result)
        break;
      if (ctrlc_pressed)
        break;
    }
    catch (trap_t t) {}

    if (picoc_always_counter != 0)
      picoc_eval_always();

    set_procs_debug(false);
    step(1);
  }
  
  printf("[HALT]\n");
}
