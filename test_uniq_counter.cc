#include <cstdio>

#include <random>
#include <unordered_map>

#include "uniq_counter.h"

bool oracle_test() {
  Counter counter(128);
  std::unordered_map<uint64_t, uint64_t> oracle;

  std::random_device r;
  std::mt19937_64 gen(r());
  // make sure we exercise the mod function
  std::uniform_int_distribution<uint64_t> d(10000, 10127);

  for (int i = 0; i < 100000; i++) {
    uint64_t v = d(gen);

    oracle[v] = oracle[v] + 1;

    counter.incr(v);
  }

  // make sure every key in our counter has the same value as the oracle
  for (auto iter : oracle) {
    entry_t *v = counter.get(iter.first);
    if (v->addr != iter.first) {
      fprintf(stdout, "expected address %lu to equal %lu\n", iter.first,
              v->addr);
      return false;
    }

    if (v->count != iter.second) {
      fprintf(stdout, "expected key %lu to be %lu. got %lu instead.\n",
              iter.first, iter.second, v->count);
      return false;
    }
  }

  //  cout << "useful stats:" << endl;
  //  cout << "\toracle load factor: " << oracle.load_factor() << endl;
  //  cout << endl;
  //
  return true;
}

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

int main() {
  if (!uniq_counter_test_main())
    return 1;

  if (!oracle_test())
    return 1;

  fprintf(stdout, "passed.\n");

  return 0;
}
