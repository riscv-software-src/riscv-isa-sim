#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "../../obj_dir_tb_1/tb_1__Dpi.h"
#include "../../obj_dir_tb_2/tb_2__Dpi.h"
#include "../../obj_dir_tb_3/tb_3__Dpi.h"
#include "../../obj_dir_tb_4/tb_4__Dpi.h"

#include "common.h"

extern int get_last_commit();

int ind;
data_t last_commit;

FILE *out_logs, *err_logs;

void init_out_err_files()
{
#define OUT_FILE_LOC "/home/usr1/riscv-isa-sim/a_tets_faruk/verilator_verilog/log/out_log.txt"
#define ERR_FILE_LOC "/home/usr1/riscv-isa-sim/a_tets_faruk/verilator_verilog/log/err_log.txt"

  FILE *temp;
  if ((temp = fopen(OUT_FILE_LOC, "w")) == NULL)
  {
  }
  fclose(temp);
  if ((temp = fopen(ERR_FILE_LOC, "w")) == NULL)
  {
  }
  fclose(temp);
  // files are cleared
  if ((out_logs = fopen(OUT_FILE_LOC, "a")) == NULL)
  {
  }
  if ((err_logs = fopen(ERR_FILE_LOC, "a")) == NULL)
  {
  }

#undef OUT_FILE_LOC
#undef ERR_FILE_LOC

  char time_str[100];
  time_t now = time(NULL);
  strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
  // fprintf(out_logs, "---------------------------------\n");
  fprintf(out_logs, "[%s] out_logs ve err_logs olusturuldu\n", time_str);
  // fflush(out_logs);
}

int get_last_commit()
{
  return last_commit;
}

int print_flush_out(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int ret_val = vfprintf(out_logs, fmt, args);
  va_end(args);
  fflush(out_logs);
  return ret_val;
}

int print_flush_out_dated(const char *fmt, ...)
{
  char time_str[100];
  time_t now = time(NULL);
  strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
  fprintf(out_logs,"[%s] ",time_str);
  
  va_list args;
  va_start(args, fmt);
  int ret_val = vfprintf(out_logs, fmt, args);
  va_end(args);
  fflush(out_logs);
  return ret_val;
}

int print_flush_err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int ret_val = vfprintf(err_logs, fmt, args);
  va_end(args);
  fflush(err_logs);
  return ret_val;
}


int print_flush_err_dated(const char *fmt, ...)
{
  char time_str[100];
  time_t now = time(NULL);
  strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
  fprintf(err_logs,"[%s] ",time_str);

  va_list args;
  va_start(args, fmt);
  int ret_val = vfprintf(err_logs, fmt, args);
  va_end(args);
  fflush(err_logs);
  return ret_val;
}
