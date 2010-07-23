#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include "common.h"
#include "load_elf.h"

void load_elf(const char* buf, size_t size, loader_t* loader)
{
  demand(size >= sizeof(Elf64_Ehdr), "truncated ELF!");
  const Elf64_Ehdr* eh = (const Elf64_Ehdr*)buf;

  const uint32_t* magic = (const uint32_t*)eh->e_ident;
  demand(*magic == *(const uint32_t*)ELFMAG, "not a host-endian ELF!");
  demand(size >= eh->e_phoff + eh->e_phnum*sizeof(Elf64_Phdr), "bad ELF!");
  const Elf64_Phdr* phs = (const Elf64_Phdr*)(buf+eh->e_phoff);

  for(int i = 0; i < eh->e_phnum; i++)
  {
    const Elf64_Phdr* ph = &phs[i];
    if(ph->p_type == SHT_PROGBITS && ph->p_memsz)
    {
      demand(size >= ph->p_offset + ph->p_filesz, "truncated ELF!");
      demand(ph->p_memsz >= ph->p_filesz, "bad ELF!");

      loader->write(ph->p_vaddr, ph->p_filesz, buf + ph->p_offset);
      loader->write(ph->p_vaddr + ph->p_filesz, ph->p_memsz - ph->p_filesz);

      printf("%d\n", ph->p_vaddr);
    }
  }
}

void load_elf(const char* fn, loader_t* loader)
{
  int fd = open(fn, O_RDONLY);
  demand(fd != -1, "couldn't open %s", fn);

  struct stat s;
  demand(fstat(fd,&s) != -1, "couldn't stat %s", fn);

  char* addr = (char*)mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  demand(addr != MAP_FAILED, "couldn't mmap %s", fn);

  close(fd);

  load_elf(addr, s.st_size, loader);
}
