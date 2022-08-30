#ifndef UNIQ_COUNTER_H
#define UNIQ_COUNTER_H

#include <cstdint>

struct entry_t {
  uint64_t addr;
  uint64_t count;
};

struct Counter {
  entry_t *entries;
  uint32_t nbuckets;

  Counter(uint32_t nbuckets);
  ~Counter();

  entry_t *get(uint64_t key) { return &entries[key % nbuckets]; }
  void incr(uint64_t key);
};

#ifdef TEST
bool uniq_counter_test_main(void);
#endif

#endif
