#include "dynamo/ring.hpp"
#include "dynamo/hash.hpp"

#include <stdexcept>

namespace dynamo {

void ConsistentHashRing::build(const std::vector<Node>& nodes, int vnodes) {
  if (nodes.empty()) throw std::runtime_error("cannot build ring with 0 nodes");
  if (vnodes <= 0) throw std::runtime_error("vnodes must be > 0");

  nodes_ = nodes;
  ring_.clear();

  for (size_t i = 0; i < nodes_.size(); ++i) {
    for (int v = 0; v < vnodes; ++v) {
      const std::string point = nodes_[i].id + "#" + std::to_string(v);
      const uint64_t h = fnv1a_64(point);
      ring_[h] = i;
    }
  }
}

size_t ConsistentHashRing::pick_node_index(const std::string& key) const {
  if (ring_.empty()) throw std::runtime_error("ring not built");
  const uint64_t h = fnv1a_64(key);
  auto it = ring_.lower_bound(h);
  if (it == ring_.end()) it = ring_.begin();
  return it->second;
}

std::vector<size_t> ConsistentHashRing::ordered_node_indices(const std::string& key) const {
  if (nodes_.empty()) throw std::runtime_error("ring not built");
  std::vector<size_t> order;
  order.reserve(nodes_.size());

  const size_t primary = pick_node_index(key);
  order.push_back(primary);

  // add remaining unique nodes in a stable order (just rotate the nodes_ vector)
  for (size_t i = 1; i < nodes_.size(); ++i) {
    order.push_back((primary + i) % nodes_.size());
  }
  return order;
}

} // namespace dynamo
