#include "dynamo/client.hpp"
#include "dynamo/config.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static void usage() {
  std::cout <<
R"(dynamoctl - C++ client for Mini Dynamo

Usage:
  dynamoctl [--config PATH] [--timeout-ms N] [--retries N] put <key> <value>
  dynamoctl [--config PATH] [--timeout-ms N] [--retries N] get <key>
  dynamoctl [--config PATH] health

Config:
  - If --config is omitted or the file doesn't exist, defaults to localhost:9001-9003.
  - Config formats supported:
      JSON: { "vnodes": 128, "nodes": [ {"id":"n1","host":"localhost","port":9001}, ... ] }
      JSON: { "nodes": [ {"id":"n1","address":"localhost:9001"}, ... ] }
      Text:
        vnodes=128
        n1 localhost 9001
        n2 localhost 9002
        n3 localhost 9003
)";
}

static bool starts_with(const std::string& s, const std::string& p) {
  return s.rfind(p, 0) == 0;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    usage();
    return 2;
  }

  std::string config_path = "nodes.docker.json";
  uint32_t timeout_ms = 2000;
  int retries = 2;

  std::vector<std::string> args;
  args.reserve(argc - 1);
  for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);

  // Parse flags (very small parser)
  for (;;) {
    if (args.size() >= 2 && args[0] == "--config") {
      config_path = args[1];
      args.erase(args.begin(), args.begin() + 2);
    } else if (args.size() >= 2 && args[0] == "--timeout-ms") {
      timeout_ms = static_cast<uint32_t>(std::stoul(args[1]));
      args.erase(args.begin(), args.begin() + 2);
    } else if (args.size() >= 2 && args[0] == "--retries") {
      retries = std::stoi(args[1]);
      args.erase(args.begin(), args.begin() + 2);
    } else {
      break;
    }
  }

  if (args.empty()) {
    usage();
    return 2;
  }

  const std::string cmd = args[0];

  dynamo::ClusterConfig cfg;
  try {
    cfg = dynamo::ClusterConfig::load_from_file(config_path);
  } catch (const std::exception& e) {
    std::cerr << "Config error: " << e.what() << "\n";
    return 1;
  }

  dynamo::HttpClient http(timeout_ms);
  dynamo::RetryPolicy rp;
  rp.max_retries = retries;

  dynamo::DynamoClient client(std::move(cfg), std::move(http), rp);

  if (cmd == "put") {
    if (args.size() != 3) {
      usage();
      return 2;
    }
    const auto& key = args[1];
    const auto& val = args[2];
    const bool ok = client.put(key, val);
    if (!ok) {
      std::cerr << "PUT failed\n";
      return 1;
    }
    std::cout << "OK\n";
    return 0;
  }

  if (cmd == "get") {
    if (args.size() != 2) {
      usage();
      return 2;
    }
    const auto& key = args[1];
    const auto val = client.get(key);
    if (!val.has_value()) {
      std::cerr << "NOT_FOUND_OR_ERROR\n";
      return 1;
    }
    std::cout << *val << "\n";
    return 0;
  }

  if (cmd == "health") {
    const auto lines = client.health_all();
    for (const auto& l : lines) std::cout << l << "\n";
    return 0;
  }

  usage();
  return 2;
}
