#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "tinyelf.h"

int main(void) {
  const char *filename = "../pikuma3d/obj_parser_test";

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("Could not open file");
    return EXIT_FAILURE;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    perror("Could not stat file");
    return EXIT_FAILURE;
  }

  if (sb.st_size < 0) {
    fprintf(stderr, "Something has gone wrong (negative file size).\n");
    return EXIT_FAILURE;
  }

  size_t len = (size_t)sb.st_size;

  fprintf(stdout, "mapping %ld bytes from file %s\n", len, filename);
  uint8_t *binary = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (binary == MAP_FAILED) {
    perror("Could not mmap file");
    return EXIT_FAILURE;
  }

  if (close(fd) == -1) {
    perror("Could not close file");
    return EXIT_FAILURE;
  }

  if (!elf_check(binary, len)) {
    fprintf(stderr, "Error: not an ELF file we can use.\n");
    return EXIT_FAILURE;
  }

  elf_header_debug_print(binary);

  elf_dwarf_sections(binary);

  return EXIT_SUCCESS;
}
