#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "dynamo/config.hpp"

namespace dynamo {

class ConsistentHashRing {
public:
  ConsistentHashRing() = default;

  void build(const std::vector<Node>& nodes, int vnodes);

  // Returns index into nodes_ for the node responsible for key.
  // Precondition: build() has been called with at least 1 node.
  size_t pick_node_index(const std::string& key) const;

  // Returns ordered list of node indices to try for key:
  // primary first, then the remaining nodes in ring order (wrap-around).
  std::vector<size_t> ordered_node_indices(const std::string& key) const;

private:
  std::vector<Node> nodes_;
  std::map<uint64_t, size_t> ring_; // hash point -> node index
};

} // namespace dynamo
