// See LICENSE for details.

#ifndef _ELF_H
#define _ELF_H

#include <stdint.h>

#define ET_EXEC 2
#define ET_DYN 3
#define EM_RISCV 243
#define EM_NONE 0
#define EV_CURRENT 1

#define IS_ELF(hdr) \
  ((hdr).e_ident[0] == 0x7f && (hdr).e_ident[1] == 'E' && \
   (hdr).e_ident[2] == 'L'  && (hdr).e_ident[3] == 'F')

#define ELF_SWAP(hdr, val) (IS_ELFLE(hdr)? from_le((val)) : from_be((val)))

#define IS_ELF32(hdr) (IS_ELF(hdr) && (hdr).e_ident[4] == 1)
#define IS_ELF64(hdr) (IS_ELF(hdr) && (hdr).e_ident[4] == 2)
#define IS_ELFLE(hdr) (IS_ELF(hdr) && (hdr).e_ident[5] == 1)
#define IS_ELFBE(hdr) (IS_ELF(hdr) && (hdr).e_ident[5] == 2)
#define IS_ELF_EXEC(hdr) (IS_ELF(hdr) && ELF_SWAP((hdr), (hdr).e_type) == ET_EXEC)
#define IS_ELF_DYN(hdr) (IS_ELF(hdr) && ELF_SWAP((hdr), (hdr).e_type) == ET_DYN)
#define IS_ELF_RISCV(hdr) (IS_ELF(hdr) && ELF_SWAP((hdr), (hdr).e_machine) == EM_RISCV)
#define IS_ELF_EM_NONE(hdr) (IS_ELF(hdr) && ELF_SWAP((hdr), (hdr).e_machine) == EM_NONE)
#define IS_ELF_VCURRENT(hdr) (IS_ELF(hdr) && ELF_SWAP((hdr), (hdr).e_version) == EV_CURRENT)

#define PT_LOAD 1

#define SHT_NOBITS 8

typedef struct {
  uint8_t  e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint32_t e_entry;
  uint32_t e_phoff;
  uint32_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
  uint32_t sh_name;
  uint32_t sh_type;
  uint32_t sh_flags;
  uint32_t sh_addr;
  uint32_t sh_offset;
  uint32_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint32_t sh_addralign;
  uint32_t sh_entsize;
} Elf32_Shdr;

typedef struct
{
  uint32_t p_type;
  uint32_t p_offset;
  uint32_t p_vaddr;
  uint32_t p_paddr;
  uint32_t p_filesz;
  uint32_t p_memsz;
  uint32_t p_flags;
  uint32_t p_align;
} Elf32_Phdr;

typedef struct
{
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
} Elf32_Sym;

typedef struct {
  uint8_t  e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
} Elf64_Shdr;

typedef struct {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
} Elf64_Phdr;

typedef struct {
  uint32_t st_name;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
  uint64_t st_value;
  uint64_t st_size;
} Elf64_Sym;

#endif
