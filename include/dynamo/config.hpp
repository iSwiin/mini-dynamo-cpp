#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace dynamo {

struct Node {
  std::string id;
  std::string host;
  uint16_t port{0};

  std::string base_url() const; // e.g., http://localhost:9001
};

struct ClusterConfig {
  std::vector<Node> nodes;
  int vnodes{128}; // virtual nodes per physical node

  // Loads a minimal subset of JSON or a simple text config.
  // JSON supported shapes:
  //   { "vnodes": 128, "nodes": [ {"id":"n1","host":"localhost","port":9001}, ... ] }
  //   { "nodes": [ {"id":"n1","address":"localhost:9001"}, ... ] }
  //
  // Text supported shape:
  //   vnodes=128
  //   n1 localhost 9001
  //   n2 localhost 9002
  //   n3 localhost 9003
  static ClusterConfig load_from_file(const std::string& path);

  // Convenience default for local dev if no config file exists.
  static ClusterConfig local_default();
};

} // namespace dynamo
