#include "dynamo/config.hpp"
#include <fstream>

void check(bool cond, const char* expr, const char* file, int line);
#define CHECK(x) check((x), #x, __FILE__, __LINE__)

static std::string tmp_path() {
#ifdef _WIN32
  return "tmp_nodes.txt";
#else
  return "/tmp/tmp_nodes.txt";
#endif
}

void test_config() {
  const std::string path = tmp_path();
  {
    std::ofstream out(path);
    out << "vnodes=64\n";
    out << "n1 localhost 9001\n";
    out << "n2 localhost 9002\n";
    out << "n3 localhost 9003\n";
  }
  const auto cfg = dynamo::ClusterConfig::load_from_file(path);
  CHECK(cfg.vnodes == 64);
  CHECK(cfg.nodes.size() == 3);
}
