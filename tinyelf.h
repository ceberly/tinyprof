#ifndef TINYELF_H
#define TINYELF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool elf_check(const uint8_t *, size_t);

void elf_header_debug_print(const uint8_t *);
void elf_dwarf_sections(const uint8_t *);

#endif
