#include <iostream>
#include <stdexcept>

int g_failures = 0;

void check(bool cond, const char* expr, const char* file, int line) {
  if (!cond) {
    std::cerr << file << ":" << line << " CHECK failed: " << expr << "\n";
    ++g_failures;
  }
}

#define CHECK(x) check((x), #x, __FILE__, __LINE__)

int main() {
  extern void test_hash();
  extern void test_ring();
  extern void test_config();

  test_hash();
  test_ring();
  test_config();

  if (g_failures == 0) {
    std::cout << "ALL_TESTS_PASSED\n";
    return 0;
  }
  std::cerr << g_failures << " test(s) failed\n";
  return 1;
}
