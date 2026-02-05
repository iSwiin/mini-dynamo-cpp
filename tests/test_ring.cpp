#include "dynamo/ring.hpp"
#include <string>

void check(bool cond, const char* expr, const char* file, int line);
#define CHECK(x) check((x), #x, __FILE__, __LINE__)

void test_ring() {
  dynamo::ConsistentHashRing ring;
  std::vector<dynamo::Node> nodes = {
    {"n1", "localhost", 9001},
    {"n2", "localhost", 9002},
    {"n3", "localhost", 9003},
  };
  ring.build(nodes, 32);

  const size_t a = ring.pick_node_index("alpha");
  const size_t b = ring.pick_node_index("alpha");
  CHECK(a == b);

  const auto order = ring.ordered_node_indices("alpha");
  CHECK(order.size() == nodes.size());
  CHECK(order[0] == a);
}
