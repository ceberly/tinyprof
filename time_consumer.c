#include <stdio.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <time.h>
#endif

int random_int() {
#ifdef __linux__
  return rand();
#endif
}

#define NNODES 100000

int main(void) {
#ifdef __linux__
  srand(time(NULL));
#endif

  // Allocate random sized buffers, fill them with 'a' characters,
  // then search for the longest such buffer using strlen.

  char *a_buffer = calloc(USHRT_MAX, sizeof(char));
  if (a_buffer == NULL) {
    fprintf(stderr, "out of memory.\n");
    return EXIT_FAILURE;
  }

  memset(a_buffer, 'a', USHRT_MAX);

  char **nodes = calloc(NNODES, sizeof(char *));
  if (nodes == NULL) {
    fprintf(stderr, "out of memory\n.");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < NNODES; i++) {
    int r = random_int();
    unsigned short len = r % USHRT_MAX;
    if (len == 0)
      len = 1;

    char *node = calloc(len, sizeof(char));
    if (node == NULL) {
      fprintf(stderr, "out of memory.\n");
      return EXIT_FAILURE;
    }

    memcpy(node, a_buffer, len - 1);

    nodes[i] = node;
  }

  unsigned short largestn = 0;
  for (int i = 0; i < NNODES; i++) {
    unsigned short len = strlen(nodes[i]);
    if (len > largestn)
      largestn = len;
  }

  printf("largest n: %d\n", largestn);

  return EXIT_SUCCESS;
}
