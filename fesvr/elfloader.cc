// See LICENSE for license details.

#include "elf.h"
#include "memif.h"
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>

std::map<std::string, uint64_t> load_elf(const char* fn, memif_t* memif, reg_t* entry)
{
  int fd = open(fn, O_RDONLY);
  struct stat s;
  assert(fd != -1);
  if (fstat(fd, &s) < 0)
    abort();
  size_t size = s.st_size;

  char* buf = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(buf != MAP_FAILED);
  close(fd);

  assert(size >= sizeof(Elf64_Ehdr));
  const Elf64_Ehdr* eh64 = (const Elf64_Ehdr*)buf;
  assert(IS_ELF32(*eh64) || IS_ELF64(*eh64));

  std::vector<uint8_t> zeros;
  std::map<std::string, uint64_t> symbols;

  #define LOAD_ELF(ehdr_t, phdr_t, shdr_t, sym_t) do { \
    ehdr_t* eh = (ehdr_t*)buf; \
    phdr_t* ph = (phdr_t*)(buf + eh->e_phoff); \
    *entry = eh->e_entry; \
    assert(size >= eh->e_phoff + eh->e_phnum*sizeof(*ph)); \
    for (unsigned i = 0; i < eh->e_phnum; i++) { \
      if(ph[i].p_type == PT_LOAD && ph[i].p_memsz) { \
        if (ph[i].p_filesz) { \
          assert(size >= ph[i].p_offset + ph[i].p_filesz); \
          memif->write(ph[i].p_paddr, ph[i].p_filesz, (uint8_t*)buf + ph[i].p_offset); \
        } \
        zeros.resize(ph[i].p_memsz - ph[i].p_filesz); \
        memif->write(ph[i].p_paddr + ph[i].p_filesz, ph[i].p_memsz - ph[i].p_filesz, &zeros[0]); \
      } \
    } \
    shdr_t* sh = (shdr_t*)(buf + eh->e_shoff); \
    assert(size >= eh->e_shoff + eh->e_shnum*sizeof(*sh)); \
    assert(eh->e_shstrndx < eh->e_shnum); \
    assert(size >= sh[eh->e_shstrndx].sh_offset + sh[eh->e_shstrndx].sh_size); \
    char *shstrtab = buf + sh[eh->e_shstrndx].sh_offset; \
    unsigned strtabidx = 0, symtabidx = 0; \
    for (unsigned i = 0; i < eh->e_shnum; i++) { \
      unsigned max_len = sh[eh->e_shstrndx].sh_size - sh[i].sh_name; \
      assert(sh[i].sh_name < sh[eh->e_shstrndx].sh_size); \
      assert(strnlen(shstrtab + sh[i].sh_name, max_len) < max_len); \
      if (sh[i].sh_type & SHT_NOBITS) continue; \
      assert(size >= sh[i].sh_offset + sh[i].sh_size); \
      if (strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) \
        strtabidx = i; \
      if (strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) \
        symtabidx = i; \
    } \
    if (strtabidx && symtabidx) { \
      char* strtab = buf + sh[strtabidx].sh_offset; \
      sym_t* sym = (sym_t*)(buf + sh[symtabidx].sh_offset); \
      for (unsigned i = 0; i < sh[symtabidx].sh_size/sizeof(sym_t); i++) { \
        unsigned max_len = sh[strtabidx].sh_size - sym[i].st_name; \
        assert(sym[i].st_name < sh[strtabidx].sh_size); \
        assert(strnlen(strtab + sym[i].st_name, max_len) < max_len); \
        symbols[strtab + sym[i].st_name] = sym[i].st_value; \
      } \
    } \
  } while(0)

  if (IS_ELF32(*eh64))
    LOAD_ELF(Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym);
  else
    LOAD_ELF(Elf64_Ehdr, Elf64_Phdr, Elf64_Shdr, Elf64_Sym);

  munmap(buf, size);

  return symbols;
}
