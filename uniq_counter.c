#include "uniq_counter.h"

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

struct counter_impl {
  entry_t *entries;
  uint32_t nbuckets;
};

struct counter_impl *new_counter(uint32_t nbuckets) {
  if (__builtin_popcount(nbuckets) != 1) {
#ifndef TEST
    // silence this message during tests, where we expect it.
    fprintf(stderr, "abort: nbuckets must be a power of two.\n");
#endif
    return NULL;
  }

  struct counter_impl *c = calloc(1, sizeof(struct counter_impl));
  if (c == NULL)
    return NULL;

  entry_t *entries = calloc(nbuckets, sizeof(entry_t));
  if (entries == NULL)
    return NULL;

  c->nbuckets = nbuckets;
  c->entries = entries;

  return c;
}

void counter_destroy(struct counter_impl *counter) {
  free(counter->entries);
  free(counter);
}

inline entry_t *counter_get(struct counter_impl *counter, uint64_t key) {
  return &counter->entries[key % counter->nbuckets];
}

void counter_incr(struct counter_impl *counter, uint64_t key) {
  // mod a power of 2 is very fast.
  uint32_t bucket = key % counter->nbuckets;

  entry_t entry = counter->entries[bucket];

  // we should never hash more than one address to an entry.
  if (__builtin_expect(entry.addr > 0 && entry.addr != key, 0)) {
#ifdef TEST
    return;
#endif
    // in the test, we just make sure no new value was added.
    fprintf(stderr, "hash table overflow.\n");
    assert(false);
  }

  entry.addr = key;
  entry.count = entry.count + 1;

  counter->entries[bucket] = entry;
}

#ifdef TEST
#include <stdio.h>

typedef struct counter_impl counter_impl;

bool uniq_counter_test_main(void) {
  // nbuckets should be a power of 2
  counter_impl *c = new_counter(100);
  if (c != NULL) {
    counter_destroy(c);
    return false;
  }

  c = new_counter(128);
  if (c == NULL)
    return false;

  // overflow should not add new values
  counter_incr(c, 10);
  counter_incr(c, 138);

  entry_t *v = counter_get(c, 138);
  if (v->addr != 10 || v->count != 1) {
    fprintf(stderr, "hash table overflowed.\n");
    counter_destroy(c);
    return false;
  }

  counter_destroy(c);
  return true;
}
#endif
