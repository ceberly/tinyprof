#include "tinyelf.h"

// better to use some kind of logger directly writing here.
#include <stdio.h>
#include <string.h>

#include <elf.h>

// possibly mixed endian values.
// typedef union {
//  uint8_t b[2];
//  uint16_t v;
//} two_byte_t;
//
// typedef union {
//  uint8_t b[4];
//  uint32_t v;
//} four_byte_t;
//
// typedef union {
//  uint8_t b[8];
//  uint64_t v;
//} eight_byte_t;

typedef uint16_t two_byte_t;
typedef uint32_t four_byte_t;
typedef uint64_t eight_byte_t;

typedef struct {
  uint8_t magic_number[4];
  uint8_t format_class; // 32 or 64 bit format.
  uint8_t endian_class;
  uint8_t version;
  uint8_t os_abi;
  uint8_t abi_version;
  uint8_t pad[7];
  two_byte_t obj_type;
  two_byte_t isa_type;
  four_byte_t further_version;
} elf_header_first_part;

typedef struct {
  four_byte_t entry;
  four_byte_t program_header_table;
  four_byte_t section_header_table;
} elf_header_addrs_32;

typedef struct {
  eight_byte_t entry;
  eight_byte_t program_header_table;
  eight_byte_t section_header_table;
} elf_header_addrs_64;

typedef elf_header_first_part elf_header;

// check that we can do something useful with the elf file pointed to by buffer.
bool elf_check(const uint8_t *buffer, size_t len) {
  if (len < 0x40) // header size for 64 bit elf header
    return false;

  if (buffer[0] != 0x7f) // magic
    return false;

  if (buffer[1] != 0x45) // E
    return false;

  if (buffer[2] != 0x4c) // L
    return false;

  if (buffer[3] != 0x46) // F
    return false;

  if (buffer[4] != 0x2) // 64 bit only
    return false;

  if (buffer[5] != 0x1) // little endian only
    return false;

  if (buffer[6] != 0x1) // only allowed ELF version
    return false;

  if (buffer[7] != 0x0) // system v ABI
    return false;

  // 1 byte further abi version
  // 7 byte padding

  // executable or shared object (also an executable sometimes)
  if (((two_byte_t)buffer[16] != 0x2) && ((two_byte_t)buffer[16] != 0x3))
    return false;

  if ((two_byte_t)buffer[18] != 0x3e) // x86_64 ISA
    return false;

  // 4 byte elf version, again.

  // we need a program with an entry point (non zero address) for now.
  if ((eight_byte_t)buffer[24] == 0x0)
    return false;

  eight_byte_t section_header = (eight_byte_t)(buffer[0x28]);

  two_byte_t section_header_size = (two_byte_t)(buffer[0x3a]);
  if (section_header_size != 0x40) // we should be reading a 64 bit file
    return false;

  two_byte_t nsections = (two_byte_t)(buffer[0x3c]);

  // can we read all the sections?
  if (len < (section_header + 0x40 * nsections))
    return false;

  return true;
}

// All the functions below assume that we have an acceptable ELF file.
// You should run elf_check() before using them!

// print some interesting metadata
void elf_header_debug_print(const uint8_t *buffer) {
  elf_header_first_part first_part;
  memcpy(&first_part, buffer, sizeof(elf_header_first_part));

  elf_header_addrs_64 addrs;
  memcpy(&addrs, buffer + sizeof(elf_header_first_part),
         sizeof(elf_header_addrs_64));

  fprintf(stdout, "%x", first_part.magic_number[0]);
  fprintf(stdout, " %c", first_part.magic_number[1]);
  fprintf(stdout, "%c", first_part.magic_number[2]);
  fprintf(stdout, "%c\n", first_part.magic_number[3]);

  fprintf(stdout, "\t format: %s\n",
          first_part.format_class == 1 ? "32 bit" : "64 bit");
  fprintf(stdout, "\t endian: %s\n",
          first_part.endian_class == 1 ? "little" : "big");
  fprintf(stdout, "\t elf version: %x\n", first_part.version);
  fprintf(stdout, "\t OS ABI: %x\n", first_part.os_abi);

  fprintf(stdout, "\t OBJ file type: %x\n", first_part.obj_type);
  fprintf(stdout, "\t ISA type: %x\n", first_part.isa_type);

  fprintf(stdout, "\t entry addr: %lx\n", addrs.entry);
  fprintf(stdout, "\t program header addr: %ld\n", addrs.program_header_table);
  fprintf(stdout, "\t section header addr: %ld\n", addrs.section_header_table);

  fprintf(stdout, "\n");
}

void elf_dwarf_sections(const uint8_t *buffer) {
  // reminder: the validity of these reads is checked by elf_check() :)

  Elf64_Ehdr *header = (Elf64_Ehdr *)buffer;

  fprintf(stderr, "section header offset: %zu, sections: %d, names: %d\n",
          header->e_shoff, header->e_shnum, header->e_shstrndx);

  Elf64_Shdr *strings_header =
      (Elf64_Shdr *)(buffer + header->e_shoff +
                     (header->e_shstrndx * header->e_shentsize));

  const char *strings_start =
      (const char *)(buffer + strings_header->sh_offset);

  for (size_t i = 0; i < strings_header->sh_size; i++) {
    fprintf(stderr, "%s\n", strings_start + i);
  }

  fprintf(stderr, "\n");
}
