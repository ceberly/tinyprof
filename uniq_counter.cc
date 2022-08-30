#include "uniq_counter.h"

#include <stdexcept>

Counter::Counter(uint32_t nbuckets) : nbuckets{nbuckets} {
  if (__builtin_popcount(nbuckets) != 1) {
#ifndef TEST
    // silence this exception during tests, where we expect it.
    throw std::runtime_error("abort: nbuckets should be a power of 2");
#endif
    entries = nullptr;
    return;
  }

  entries = new entry_t[nbuckets]();
}

Counter::~Counter() { delete[] entries; }

void Counter::incr(uint64_t key) {
  // mod a power of 2 is very fast.
  uint32_t bucket = key % nbuckets;

  entry_t entry = entries[bucket];

  // we should never hash more than one address to an entry.
  if (__builtin_expect(entry.addr > 0 && entry.addr != key, 0)) {
#ifdef TEST
    return;
#endif
    // in the test, we just make sure no new value was added.
    throw std::runtime_error("abort: hash table overflow");
  }

  entry.addr = key;
  entry.count = entry.count + 1;

  entries[bucket] = entry;
}

#ifdef TEST
#include <stdio.h>

typedef struct counter_impl counter_impl;

bool uniq_counter_test_main(void) {
  // nbuckets should be a power of 2
  Counter c(100);
  if (c.entries != NULL) {
    return false;
  }

  Counter c2(128);
  // overflow should not add new values
  c2.incr(10);
  c2.incr(138);

  entry_t *v = c2.get(138);
  if (v->addr != 10 || v->count != 1) {
    fprintf(stderr, "hash table overflowed: addr: %lu  count: %lu\n", v->addr,
            v->count);
    return false;
  }

  return true;
}
#endif
