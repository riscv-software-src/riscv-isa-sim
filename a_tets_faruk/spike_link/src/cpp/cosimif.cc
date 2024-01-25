#include "cosim_create_sim.h"
#include "../../obj_dir_tb_spike_link/tb_spike_link__Dpi.h"
#include "args_reader.h"
#include "debug_header.h"

#ifndef KEY_WIDTH
#define KEY_WIDTH 64
#endif

#ifndef VALUE_WIDTH
#define VALUE_WIDTH 128
#endif

#ifndef DPI_WIDTH
#define DPI_WIDTH 32
#endif

sim_t *simulation_object;

std::function<void()> step_callback;

// !!! bu iki variable her ne kadar init'den baska bir yerde kullanilmiyor gibi gozukse de
// aslinda step_callback'e bind edildigi icin oralarda kullaniliyor.
// dolayisiyla bu iki variable, init local scope'dan disinda da kullanilabilir olmali.
// bu yuzden global scope'ta tanimliyorum.
std::queue<reg_t> fromhost_queue;
// !!! fromhost_calback reg_t alan, void donduren bir std::function
std::function<void(reg_t)> fromhost_callback;

void init()
{
  auto argc_argv = read_args_from_file("/home/usr1/riscv-isa-sim/a_tets_faruk/spike_link/log/args.txt");

  simulation_object = create_sim_with_args(argc_argv->argc, argc_argv->argv);

  simulation_object->htif_prerun();

  simulation_object->htif_start();

  auto enq_func = [](std::queue<reg_t> *q, uint64_t x)
  { q->push(x); };

  // !!! first argument of enq_func is bind to fromhost_queue.
  // second argument is a placeholder for the reg_t argument of fromhost_callback.
  // caller passes an actual argument to the first (_1) argument of the fromhost_callback.
  fromhost_callback = std::bind(enq_func, &fromhost_queue, std::placeholders::_1);
  // !!! yani fromhost_callback, fromhost_queue'ye
  // reg_t turunden bir seyler push'lamaya yariyor

  if (simulation_object->htif_communication_available())
  {
    step_callback = std::bind(&sim_t::htif_single_step_with_communication, simulation_object, &fromhost_queue, fromhost_callback);
  }
  else
  {
    step_callback = std::bind(&sim_t::htif_single_step_without_communication, simulation_object);
  }
}

void step()
{
  step_callback();
}

/// @brief for key and value arrays: packed dimension size: dim0; num entries: dim1; entry size in packets: dim2
/// @param key_array the keys of the unordered_map is written to this array
/// @param value_array the values of the unordered_map is written to this array in same order with keys written in the key_array
/// @param num_entries_inserted is the output parameter to specify the caller how many elements are valid in the output
void get_last_commit(const svOpenArrayHandle key_array, const svOpenArrayHandle value_array, int *num_entries_inserted)
{
  auto map_from_c_side = simulation_object->get_core(0)->get_state()->log_reg_write;

  DEBUG_PRINT_WARN("burada iki tarafin boyutlari icin asertion konulabilir\n");

#define NUM_ENTRIES (*num_entries_inserted)

  NUM_ENTRIES = 0;
  // Traversing an unordered map
  for (auto x : map_from_c_side)
  {
    // x.first // 64 bit, int32 cinsinden 2 tane
    // x.second // 128 bit, int32 cinsinden 4 tane
    // key'i part part gonder
    for (int part_ind = 0; part_ind < KEY_WIDTH / DPI_WIDTH; part_ind++)
    {
      // key_array[NUM_ENTRIES][part_ind] = *(((uint32_t *)&(x.first)) + part_ind);
      auto base_ptr = (uint32_t *)&(x.first);
      svBitVec32 temp = base_ptr[part_ind];
      svPutBitArrElemVecVal(key_array, &temp, NUM_ENTRIES, part_ind);
    }

    // value'yu part part gonder
    for (int part_ind = 0; part_ind < VALUE_WIDTH / DPI_WIDTH; part_ind++)
    {
      // value_array[NUM_ENTRIES][part_ind] = *(((uint32_t *)&(x.second)) + part_ind);
      auto base_ptr = (uint32_t *)&(x.second);
      svBitVec32 temp = base_ptr[part_ind];
      svPutBitArrElemVecVal(value_array, &temp, NUM_ENTRIES, part_ind);
    }
    NUM_ENTRIES++;
  }

#undef NUM_ENTRIES
}