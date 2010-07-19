#ifndef _RISCV_LOAD_ELF_H
#define _RISCV_LOAD_ELF_H

class loader_t
{
public:
  virtual void write(size_t addr, size_t bytes, const void* src = NULL) = 0;
};

void load_elf(const char* buf, size_t size, loader_t* loader);
void load_elf(const char* fn, loader_t* loader);

#endif
