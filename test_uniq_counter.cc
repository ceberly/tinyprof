#include <iostream>
#include <random>
#include <unordered_map>

#include "uniq_counter.h"

using std::cout;
using std::endl;

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
      cout << "expected address: " << iter.first << " to equal " << v->addr
           << endl;
      return false;
    }

    if (v->count != iter.second) {
      cout << "expected key " << iter.first << " to be " << iter.second
           << ". got " << v->count << " instead." << endl;
      return false;
    }
  }

  //  cout << "useful stats:" << endl;
  //  cout << "\toracle load factor: " << oracle.load_factor() << endl;
  //  cout << endl;
  //
  return true;
}

int main() {
  if (!uniq_counter_test_main())
    return 1;

  if (!oracle_test())
    return 1;

  cout << "passed." << endl;
  return 0;
}
