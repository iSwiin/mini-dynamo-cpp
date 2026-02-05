#include "dynamo/hash.hpp"
#include <cstdint>

extern int g_failures;
#define CHECK(x) check((x), #x, __FILE__, __LINE__)
void check(bool cond, const char* expr, const char* file, int line);

void test_hash() {
  using dynamo::fnv1a_64;

  const uint64_t h1 = fnv1a_64("hello");
  const uint64_t h2 = fnv1a_64("hello");
  const uint64_t h3 = fnv1a_64("world");

  CHECK(h1 == h2);
  CHECK(h1 != h3);
}
