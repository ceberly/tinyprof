#ifndef UNIQ_COUNTER_H
#define UNIQ_COUNTER_H

#include <stdint.h>

struct counter_impl;
typedef struct counter_impl Counter;

typedef struct {
  uint64_t addr;
  uint64_t count;
} entry_t;

Counter *new_counter(uint32_t nbuckets);
void counter_destroy(Counter *);

entry_t *counter_get(Counter *, uint64_t key);
void counter_incr(Counter *, uint64_t key);

#ifdef TEST
#include <stdbool.h>
bool uniq_counter_test_main(void);
#endif

#endif
